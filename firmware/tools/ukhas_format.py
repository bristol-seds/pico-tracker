"""
Formats ukhas telemetry strings for the pico tracker.

Expects to be passed a dict, with keys 'time', 'coords',
'battery', 'solar', 'temperature' and 'satellites'.
"""

import crc16
from datetime import datetime

"""
Builds ukhas string from supplied datum
"""
def ukhas_format(datum):

    callsign = "UBSEDSX"

    # Time
    time_str = "{:02}:{:02}:{:02}".format(
        datum['time'].hour, datum['time'].minute, datum['time'].second)

    # Location
    coords = datum['coords']
    location_str = "{:.6f},{:.6f},{}".format(
        coords[0], coords[1], int(round(coords[2])))

    # Everything
    ukhas_str = "{},{},{},{},{},{},{},-1".format(
        callsign, time_str, location_str, datum['satellites'],
        datum['battery'], datum['solar'], datum['temperature']);

    # Checksum
    checksum =  "{:04X}".format(crc16.crc16xmodem(ukhas_str))

    return "$${}*{}".format(ukhas_str, checksum)
