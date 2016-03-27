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
# Define packets
#
# eqn_packets = ["PARM.Battery,Solar,Internal Temperature,GPS Satellites",
#                "UNIT.Volts,Volts,Celsius",
#                "EQNS.0,.001,0,0,.001,0,0,.1,-273.2"]
eqn_packets = ["PARM.Battery,External Temperature,Internal Temperature,GPS Satellites,GPS TTFF",
               "UNIT.Volts,Celsius,Celcius",
               "EQNS.0,.001,0,0,.1,-273.2,0,.1,-273.2"]


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
ssid =     raw_input("Which callsign to set eqns for? : ")
if not ssid:
        print "Bad SSID"
        quit()

#
# Start of the telemetry packet
#
header = ssid + '>APRS,TCPIP*::' + ssid.ljust(9) + ':'

#
# Print packets for approval
#
print
for packet in eqn_packets:
        print header + packet
print
check = raw_input("Okay to send this? [y/n]: ").lower()
print

if check[0] == 'y':
        for packet in eqn_packets:
                send_packet(callsign, password, header+packet)
else:
        print "Quit..."
        quit()
