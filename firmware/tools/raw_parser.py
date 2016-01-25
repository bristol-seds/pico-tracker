"""
This script extracts backlog data from raw aprs frames stored
in file and uploads them to habitiat.

At the very bottom the data is printed. Feel free to change
the print statement to suit the data you're interested in.
The parsed data is a list of dicts, with keys 'time', 'coords',
'battery', 'solar', 'temperature' and 'satellites'.
"""

import sys
from extract_backlog import *
from ukhas_format import *
from habitat_upload import *
from datetime import datetime


# Get the name of the input file
if len(sys.argv) >= 2:
    file_name = sys.argv[1]
else:
    file_name = raw_input("File to read (rawdata.txt): ") or "rawdata.txt"

# Get the flight number
if len(sys.argv) >= 3:
    flight_number = sys.argv[2]
else:
    flight_number = raw_input("Flight Number (xx): ") or "xx"

# Callsign
callsign = "UBSEDS"+flight_number

with open(file_name, 'r') as data_file:
    data = []

    # extract backlog
    for frame in data_file:
        datum = extract_backlog_datum(frame)
        if datum:
            if datum not in data: # unique values only
                data.append(datum)

    # Sort data lines by time
    data = sorted(data, key=lambda x: x['time'])

    # Print data
    for datum in data:
        print_datum(datum)

    # Upload data to habitat
    for datum in data:
        ukhas_str = ukhas_format(datum, callsign)
        try:
            print ukhas_str
            print habitat_upload(datum['time'], ukhas_str)
        except:
            None
