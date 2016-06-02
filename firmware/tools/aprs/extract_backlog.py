"""
This script parses backlog data from the pico tracker.
Data is expected in the form of a series of lines containing
'/A={6 digits} {6 digits}z.{18 chars}{11 chars}\n'. Anything
else will be ignored.
"""

import re
import arrow
from datetime import datetime
from datetime import timedelta
from telemetry_format import *
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
Assumes data is from the last month, as per the current machine's time
"""
def extract_time(line):
    # Capture a 6 digit string
    p = re.compile(r'(\d{6})z\S{20}')
    match = p.match(line)

    if match == None:
        return None
    else:
        # Get a arrow
        arw = arrow.get(match.group(1), 'DDHHmm')
        utcnow = arrow.utcnow()

        # Set dt year/month from current utc
        # Need to calculate year/month manually because months have varying number of days
        #
        if arw.day > utcnow.day: # from last month
            last_month = utcnow.month-1

            if last_month == 0: # Last December
                arw = arw.replace(year=utcnow.year-1, month=12)
            else:
                arw = arw.replace(year=utcnow.year, month=last_month)
        else:                   # current month
            arw = arw.replace(year=utcnow.year, month=utcnow.month)

        return arw

"""
Takes a parsed telemetry line and returns latitude, longitude and
altitude. It decodes from base 91 along the way
"""
def extract_lat_long_alt(line):
    # Capture a 4 char encoded latitude
    p = re.compile(r'\d{6}z(\S{4})(\S{4})(\S{2})\S{10}')
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
Takes a telemetry line and returns telemetry readings
It decodes from base91 along the way
"""
def extract_telemetry(line, tf, length):
    # Capture an encoded telemetry segment
    p = re.compile(r'\d{6}z\S{10}(\S{'+str(length)+'})')
    match = p.match(line)

    if match == None:
        return None
    else:
        tel = match.group(1)

        # Split into 2 char chunks
        parts = [tel[i:i+2] for i in range(0, length, 2)]

        # Extract values from base 91
        values = [base91_decode(enc) for enc in tuple(parts)]

        return tf.decode_values(values)

"""
Exracts the 'raw data' segment from a line of data; this is the
section after \d{6}z
"""
def extract_raw_data(line, length):
    # Capture the raw data segment
    p = re.compile(r'(\d{6}z\S{'+str(length)+'})\|')
    match = p.search(line)

    if match == None:
        return None
    else:
        return match.group(1)

"""
Returns a datum for the backlog in an APRS frame
"""
def extract_backlog_datum(frame, tf):
    # telemetry length
    telem_len = tf.base91_encoded_len()

    # Extract raw data string
    raw = extract_raw_data(frame, 10+telem_len)

    if raw == None:
        return None
    else:
        telem = extract_telemetry(raw, tf, telem_len)

        data = {
            'time': extract_time(raw),
            'coords': extract_lat_long_alt(raw),
        }
        data.update(telem)

        return data

"""
Prints a datum
"""
def print_datum(datum, tf):
    print "{}: {:.6f} {:.6f}, {}m {}".format(
        str(datum['time']),
        datum['coords'][0], datum['coords'][1], int(round(datum['coords'][2])),
        tf.stringify(datum))
