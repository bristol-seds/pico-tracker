#!/usr/bin/env python

# ------------------------------------------------------------------------------
# Imports
# ------------------------------------------------------------------------------

import os
import sys

sys.path.append("./tools/verification")
from verification import *
import verification_tc

from random import randint
from pynmea.streamer import NMEAStream

# ------------------------------------------------------------------------------
# Test Script
# ------------------------------------------------------------------------------

class nmea_tc:
    def __init__(self):
        self.name = self.__class__.__name__

        with open("tools/verification/tc/misc/gpslog.txt") as f:
            self.lines = f.readlines()

        self.index = 0

    def get_test(self):
        """Returns some suitable test parameters"""
        try:
            line_string = self.lines[self.index]

            params = verification_tc.struct_nmea_tc_params()
            params.buff = line_string

            return params
        except:
            return None

    def is_correct(self, params, result):
        """Returns if a result is correct for the given parameters"""

        nmeastreamer = NMEAStream()
        pynmea_obj = nmeastreamer.get_objects(data=params.buff)
        pynmea_obj = nmeastreamer.get_objects(data=params.buff)


        try:
            pynmea = pynmea_obj[0]
        except:
            pynmea = None

        if pynmea:
            #print_info(str(params.buff))
            #print_info(str(pynmea))

            if hasattr(pynmea, 'latitude') and hasattr(pynmea, 'longitude'):
                # GPGGA frame
                print_info("%s::: lat:%s,lon:%s,alt:%s"%
                           (str(pynmea),
                            pynmea.latitude,
                            pynmea.longitude,
                            pynmea.antenna_altitude
                        ))

                # Check lat/lon/alt
                if pynmea.latitude:
                    lat = float(pynmea.latitude)
                    lat *= 1 if (pynmea.lat_direction == 'N') else -1

                    if lat != float(result['lat']):
                        print_error("Latitude %f != %f"%
                                    (result['lat'], lat))
                        return False
                    else:
                        print_good("Latitude %f == %f"%
                                   (result['lat'], lat))

                if pynmea.longitude:
                    lon = float(pynmea.longitude)
                    lon *= 1 if (pynmea.lon_direction == 'E') else -1

                    if lon != float(result['lon']):
                        print_error("Longitude %f != %f"%
                                    (result['lon'], lon))
                        return False
                    else:
                        print_good("Longitude %f == %f"%
                                   (result['lon'], lon))

                if pynmea.antenna_altitude:
                    alt = float(pynmea.antenna_altitude)

                    if alt != float(result['elv']):
                        print_error("Altitude %f != %f"%
                                    (result['elv'], lon))
                        return False
                    else:
                        print_good("Altitude %f == %f"%
                                   (result['elv'], alt))



        # Move on the the next line
        self.index += 1
        return True

# ------------------------------------------------------------------------------
# Run test
# ------------------------------------------------------------------------------

#if __name__ == "__main__":

tester = samd20_test()
tester.run_test_case(nmea_tc())

if __name__ == "__main__":
    del tester
