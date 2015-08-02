"""
This script parses the raw data from the pico tracker.
At the very bottom the data is printed. Feel free to change
the print statement to suit the data you're interested in.
The parsed data is a list of dicts, with keys 'time', 'coords',
'battery', 'solar', 'temperature' and 'satellites'.

Data is expected in the form of a series of lines containing
'/A={6 digits} {6 digits}z.{18 chars}{11 chars}\n'. Anything
else will be ignored.
"""

import re
import sys
from ukhas_format import *
from habitat_upload import *
from datetime import datetime
from math import log, exp

"""
Decodes a 'base 91' encoded string
"""
def base91_decode(enc_str):
    enc_ints = [ord(x) - 33 for x in enc_str]
    powers = range(len(enc_ints))[::-1]
    return sum(x * pow(91, i) for x, i in zip(enc_ints, powers))

"""
Takes a parsed telemetry line and returns a datetime
Default year and month are 2015 and July
"""
def extract_time(line):
    # Capture a 6 digit string
    p = re.compile(r'(\d{6})z\S{18}')
    match = p.match(line)

    if match == None:
        return None
    else:
        date_str = '201507' + match.group(1)

        # Return a datetime object
        return datetime.strptime(date_str, '%Y%m%d%H%M')

"""
Takes a parsed telemetry line and returns latitude, longitude and
altitude. It decodes from base 91 along the way
"""
def extract_lat_long_alt(line):
    # Capture a 4 char encoded latitude
    p = re.compile(r'\d{6}z(\S{4})(\S{4})(\S{2})\S{8}')
    match = p.match(line)

    if match == None:
        return None
    else:
        enc_lat, enc_long, enc_alt = match.groups()

        # Lat/long in fractional degrees, alt in metres
        latitude =  90.0 - (base91_decode(enc_lat) / 380926.0)
        longitude = -180.0 + (base91_decode(enc_long) / 190463.0)
        altitude = exp(log(1.002) * base91_decode(enc_alt)) / 3.2808

        return (latitude, longitude, altitude)

"""
Takes a parsed telemetry line and returns readings on battery, solar
temperature and satellite count. It decodes from base91 along the
way
"""
def extract_telemetry(line):
    # Capture an 8 char encoded telemetry segment
    p = re.compile(r'\d{6}z\S{10}(\S{8})')
    match = p.match(line)

    if match == None:
        return None
    else:
        tel = match.group(1)

        # Split into 2 char chunks
        parts = [tel[i:i+2] for i in range(0, 8, 2)]
        batt_enc, sol_enc, temp_enc, sat_enc = tuple(parts)

        # Reverse Richard's conversions
        battery = base91_decode(batt_enc) / 1000.0
        solar   = base91_decode(sol_enc) / 1000.0
        temperature = (base91_decode(temp_enc) / 10) - 273.2
        satellite_count = base91_decode(sat_enc)

        return (battery, solar, temperature, satellite_count)

"""
Exracts the 'raw data' segment from a line of data; this is the 25
character section after /A={6 digits}
"""
def extract_raw_data(line):
    # Capture the raw data segment
    p = re.compile(r'/A=\d{6} (\S{25})\|\S{11}$')
    match = p.search(line)

    if match == None:
        return None
    else:
        return match.group(1)

#-------------------------------------------------------------------------------

if len(sys.argv) == 2:
    file_name = sys.argv[1]
else:
    file_name = raw_input("File to read (rawdata.txt): ") or "rawdata.txt"

with open(file_name, 'r') as data_file:
    data = []

    for line in data_file:
        # Extract raw data string
        raw = extract_raw_data(line)

        if raw == None:
            continue
        else:
            tele = extract_telemetry(raw)

            data.append({
                'time': extract_time(raw),
                'coords': extract_lat_long_alt(raw),
                'battery': tele[0],
                'solar': tele[1],
                'temperature': tele[2],
                'satellites': tele[3]
            })

    # Sort data lines by time
    data = sorted(data, key=lambda x: x['time'])

    # Print data
    for datum in data:
        print "%s: %s, %s" % ((str(datum['time']),) + datum['coords'][:2])

    # Upload data to habitat
    for datum in data[3:]:
        ukhas_str = ukhas_format(datum)
        try:
            print ukhas_str
            print habitat_upload(datum['time'], ukhas_str)
        except:
            None
