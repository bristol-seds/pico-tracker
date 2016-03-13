"""
Formats ukhas telemetry strings for the pico tracker.
Format as ubseds13

Expects to be passed a dict, with keys 'time',
'coords', 'battery', 'temperature_external',
'temperature_internal', 'satellites' and 'ttff'.
"""

from datetime import datetime
import crcmod

"""
Builds ukhas string from supplied datum
"""
def ukhas_format(datum, callsign):

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

    # Everything
    ukhas_str = "{},{},{},{},{},{},{},{},{}".format(
        callsign, time_str, date_str, location_str,
        datum['satellites'], datum['ttff'],
        datum['battery'], datum['temperature_e'], datum['temperature_i']);

    # Checksum
    crc16 = crcmod.mkCrcFun(0x11021, 0xFFFF, False, 0x0000)
    checksum =  "{:04X}".format(crc16(ukhas_str))

    return "$${}*{}".format(ukhas_str, checksum)
