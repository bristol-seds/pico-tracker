"""
Formats ukhas telemetry strings for the pico tracker.

Expects to be passed a dict, with keys 'time',
'coords' and telemetry values
"""

from datetime import datetime
from telemetry_format import *
import crcmod

"""
Builds ukhas string from supplied datum and telemetry format
"""
def ukhas_format(datum, tf):
    # Time of Day
    time_str = "{:02}:{:02}:{:02}".format(
        datum['time'].hour, datum['time'].minute, datum['time'].second)

    # Date
    date_str = "{:02}{:02}{:02}".format(
        datum['time'].year%100, datum['time'].month, datum['time'].day)

    # Location
    coords = datum['coords']
    location_str = "{:.6f},{:.6f},{}".format(
        coords[0], coords[1], int(round(coords[2])))

    # Telemetry
    telemetry_str = tf.ukhas_format(datum)


    # All together
    ukhas_str = "{},{},{},{},{}".format(
        tf.callsign(), time_str, date_str, location_str, telemetry_str);


    # Checksum
    crc16 = crcmod.mkCrcFun(0x11021, 0xFFFF, False, 0x0000)
    checksum =  "{:04X}".format(crc16(ukhas_str))

    return "$${}*{}".format(ukhas_str, checksum)
