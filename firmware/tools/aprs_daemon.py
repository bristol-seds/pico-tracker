"""
This script opens a tcp connection with and aprs-is server.

Uses lz1dev's aprslib
"""

import re
import sys
import aprslib
from ukhas_format import *
from habitat_upload import *
from extract_backlog import *
from datetime import datetime
from math import log, exp

# Regex for balloon callsign
callsign_re = re.compile(r'M0SBU-(\d{1,2})')

"""
Returns callsign for given SSID
"""
def callsign_from_ssid(ssid):
    if ssid == "11":
        return "UBSEDS13"
    else:
        return None

"""
Attempts to extract a backlog frame and upload it
"""
def extract_and_upload(packet, ssid):
    datum = extract_backlog_datum(packet)

    if datum: # valid backlog
        print
        print "Extracted valid backlog from M0SBU-{}:".format(ssid)
        print_datum(datum)
        print

        callsign = "UBSEDS13"#callsign_from_ssid(ssid)
        if callsign is None:
            print "No callsign match for M0SBU-{}".format(ssid)
            print
            return

        ukhas_str = ukhas_format(datum, callsign)
        print ukhas_str
        try:
            print habitat_upload(datum['time'], ukhas_str)
            print
        except:
            print "Not accepted by habitat (duplicate?)"
            print




# Rx callback
def callback(packet):
    print packet

    # Try to match our callsign
    match = callsign_re.match(packet)

    if match is not None:
        extract_and_upload(packet, match.group(1))



# Main
import logging
logging.basicConfig(level=logging.DEBUG) # level=10

print
print "Opening APRS-IS connection with aprslib."
print "debug level = 10"
print "immortal = true"
print

# Get packets for all mike-zero users. Reasonably frequent stream but not enough to overload
AIS = aprslib.IS("M0SBU-1", port=14580)
AIS.set_filter("b/M0*")

AIS.connect()
# by default `raw` is False, then each line is ran through aprslib.parse()
AIS.consumer(callback, raw=True, immortal=True)
