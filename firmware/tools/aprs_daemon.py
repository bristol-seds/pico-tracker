#!/usr/bin/env python
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

from colorama import *
from datetime import datetime
from math import log, exp

# Regex for balloon callsign
callsign_re = re.compile(r'(M0SBU|AD6AM)-(\d{1,2})')

"""
Returns callsign for given SSID
"""
def callsign_from_ssid(ssid):
    if ssid == '11':
        return 'UBSEDS14'
    else:
        return None

"""
Attempts to extract a backlog frame and upload it
"""
def extract_and_upload(packet, aprs_call, ssid):

    # Callsign Lookup
    callsign = callsign_from_ssid(ssid)
    if callsign is None:
        print Fore.RED + "No callsign match for {}-{}".format(aprs_call, ssid) + Fore.RESET
        print
        return
    else:
        print Fore.GREEN + "Packet from {} ({}-{}) ✓".format(callsign, aprs_call, ssid) + Fore.RESET
        print


    # Attempt to parse with aprslib
    try:
        pkt = aprslib.parse(packet)
        print Fore.GREEN + "Parsed with aprslib ✓" + Fore.RESET

        # Identify duplicate packets from telemetry sequence

        # Save to raw data file
        rawdata_filename = "rawdata-{}.txt".format(callsign.lower())
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
    datum = extract_backlog_datum(packet)

    if datum: # valid backlog
        print
        print Fore.CYAN +  "Extracted valid backlog from {}-{}:".format(aprs_call, ssid)
        print_datum(datum)
        print Fore.RESET

        # Habitat upload
        ukhas_str = ukhas_format(datum, callsign)
        print ukhas_str
        try:
            print Fore.CYAN + habitat_upload(datum['time'], ukhas_str) + " ✓" + Fore.RESET
            print
        except:
            print Fore.YELLOW + "Not accepted by habitat (duplicate?)" + Fore.RESET
            print




# Rx callback
def callback(packet):
    print packet

    # Try to match our callsign
    match = callsign_re.match(packet)

    if match is not None:
        extract_and_upload(packet, match.group(1), match.group(2))



# Main
import logging
logging.basicConfig(level=logging.DEBUG) # level=10

print
print "Opening APRS-IS connection with aprslib."
print "debug level = 10"
print "immortal = true"
print

# Get packets for all mike-zero / alpha-delta-six users. Reasonably frequent stream but not enough to overload
AIS = aprslib.IS("M0SBU-1", port=14580)
AIS.set_filter("b/M0*/AD6*")

AIS.connect()
# by default `raw` is False, then each line is ran through aprslib.parse()
AIS.consumer(callback, raw=True, immortal=True)
