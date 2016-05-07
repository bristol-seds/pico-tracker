#!/usr/bin/python
# coding=utf-8

"""
This script opens a tcp connection with and aprs-is server.

Uses lz1dev's aprslib
"""

import re
import sys
import aprslib
import arrow
from ukhas_format import *
from habitat_upload import *
from extract_backlog import *
from telemetry_format import *

from colorama import *
from datetime import datetime
from math import log, exp

# Regex for balloon callsign
callsign_re = re.compile(r'(M0SBU|AD6AM)-(\d{1,2})')


"""
Attempts to extract a backlog frame and upload it
"""
def extract_and_upload(packet, aprs_call, ssid):

    # Attempt to match ssid
    try:
        tf = telemetry_format_ssid(ssid)
    except ValueError:
        print Fore.RED + "No callsign match for {}-{}".format(aprs_call, ssid) + Fore.RESET
        print
        return


    # Print callsign
    callsign = tf.callsign()
    print Fore.GREEN + "Packet from {} ({}-{}) ✓".format(callsign, aprs_call, ssid) + Fore.RESET
    print


    # Attempt to parse with aprslib
    try:
        pkt = aprslib.parse(packet)
        print Fore.GREEN + "Parsed with aprslib ✓" + Fore.RESET

        # Identify duplicate packets from telemetry sequence

        # Save to raw data file
        rawdata_filename = "rawdata/{}-rawdata.txt".format(callsign.lower())
        utcnow = arrow.utcnow()
        current_utc = utcnow.format('YYYY-MM-DD HH:mm:ss') + " GMT: " # UTC and GMT are basically the same right..
        with open(rawdata_filename, 'a') as rawdata_file:
            rawdata_file.write(current_utc+packet+'\n')

            print Fore.GREEN + "(wrote to {})".format(rawdata_filename) + Fore.RESET

        print
    except (aprslib.ParseError, aprslib.UnknownFormat) as exp:
        print Fore.RED + "Error parsing with aprslib ✗" + Fore.RESET
        print


    # Backlog
    datum = extract_backlog_datum(packet, tf)

    if datum: # valid backlog
        print
        print Fore.CYAN +  "Extracted valid backlog from {}-{}:".format(aprs_call, ssid)
        print_datum(datum, tf)
        print Fore.RESET

        # Habitat upload
        ukhas_str = ukhas_format(datum, tf)
        print ukhas_str
        try:
            print Fore.CYAN + str(habitat_upload(datum['time'], ukhas_str)) + " ✓" + Fore.RESET
            print
        except Exception,e:
            print Fore.YELLOW + "Not accepted by habitat (duplicate?)" + Fore.RESET
            print "Error was " + str(e)
            print




#
# Called when a packet is received through APRS-IS
#
def callback(packet):
    print packet

    # Try to match our callsign
    match = callsign_re.match(packet)

    if match is not None:
        extract_and_upload(packet, match.group(1), match.group(2))


#
# Main
#
import logging
logging.basicConfig(level=logging.DEBUG) # level=10

print
print "Opening APRS-IS connection with aprslib."
print "debug level = 10"
print "immortal = true"
print

# Get packets for m0sbu/ad6am. Reasonably frequent stream but not enough to overload
AIS = aprslib.IS("M0SBU-1", port=14580)
AIS.set_filter("b/M0SBU*/AD6AM*")

AIS.connect()
# by default `raw` is False, then each line is ran through aprslib.parse()
AIS.consumer(callback, raw=True, immortal=True)
