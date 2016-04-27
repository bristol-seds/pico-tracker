#!/usr/bin/python
#
# Uploads APRSISCE telemetry equation packets to APRS-IS
#
# Useful Resources:
# http://aprsisce.wikidot.com/doc:telemetry
# http://www.jmalsbury.com/techwiki/index.php?title=Uploading_ARPS_Frames_to_APRS-IS_-_Part_I
# http://www.aprs.org/doc/APRS101.PDF (Section 13 Telemetry Data)
#
# Based on example from John Malsbury https://github.com/jmalsbury/aprs-post
# Originial Header:
# Slight modifications to original code provided by Pete Loveall AE5PL
# Source: http://www.tapr.org/pipermail/aprssig/2007-April/018541.html

import sys, time
from socket import *
from telemetry_format import *

serverHost = 'second.aprs.net'
serverPort = 20157

#
# Sends packet to second.aprs.net
#
def send_packet(callsign, password, packet):
    # Create socket & connect to server
    sSock = socket(AF_INET, SOCK_STREAM)
    sSock.connect((serverHost, serverPort))

    # Login
    sSock.send('user ' + callsign + ' pass ' + password + ' vers "Python Script" \n')

    # Send packet
    sSock.send(packet + '\n')
    print("Sent: " + packet)

    # Close socket -- must be closed to avoid buffer overflow
    sSock.shutdown(0)
    sSock.close()

#
# Attempt to read default callsign and password from a file called
# aprs_id
#
def attempt_read_aprs_id():
    try:
        with open('aprs_id', 'r') as aprs_id:
            if aprs_id:
                print "( loaded aprs_id )"
                c = aprs_id.readline().strip()
                p = aprs_id.readline().strip()
                return c,p
    except:
        None

    print "(no aprs_id found)"
    return "",""



#
# Read callsign / password defaults?
#
print
default_callsign, default_password = attempt_read_aprs_id()

#
# User input
#
print
callsign = raw_input("Please enter your callsign{}: ".format(
    " ("+default_callsign+")")) or default_callsign
if not callsign:
    print "Bad callsign"
    quit()

password = raw_input("Please enter your password{}: ".format(
    " ("+default_password+")")) or default_password
if not password:
    print "Bad password"
    quit()

print


test_packet = raw_input("Please enter test packet: ")
if not test_packet:
    print "Bad test packet"
    quit()

print

#
# Print packet for approval
print test_packet
print

check = raw_input("Okay to send this? [y/n]: ").lower()
print

if check[0] == 'y':
    send_packet(callsign, password, test_packet)
    print "Sent..."
else:
    print "Quit..."
    quit()
