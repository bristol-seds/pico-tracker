#!/usr/bin/env python

# ------------------------------------------------------------------------------
# Imports
# ------------------------------------------------------------------------------

import sys
sys.path.append("./test")
import main

from random import randint

# ------------------------------------------------------------------------------
# Test Script
# ------------------------------------------------------------------------------

class location_aprs_tc:
    def __init__(self):
        self.name = self.__class__.__name__

        # Load CSV file into memory
        self.locations = []
        with open("./test/tc/location_aprs_tc_list.csv") as f:
            for line in f:
                if line[0] is not "#" and len(line) is not 0:
                    parts = [x.strip() for x in line.split(',')]
                    if len(parts) is 4:
                        self.locations.append({
                            'name': parts[0],
                            'frequency': float(parts[1]),
                            'lat': float(parts[2]),
                            'lon': float(parts[3])
                        })
        self.locations_index = 0


    def get_test(self):
        """Returns some suitable test parameters"""
        params = main.struct_location_aprs_tc_params()

        if self.locations_index >= len(self.locations):
            return None

        params.lon = self.locations[self.locations_index]['lon']
        params.lat = self.locations[self.locations_index]['lat']

        return params

    def is_correct(self, params, result, print_info):
        """Returns if a result is correct for the given parameters"""

        name = self.locations[self.locations_index]['name']
        expected_freq = self.locations[self.locations_index]['frequency']
        self.locations_index += 1

        # What frequency did we return?
        freq = float(result['frequency']) / (1000*1000)

        if freq == expected_freq:
            print_info("{}: {:.3f} MHz".format(name, freq))
            return True
        else:
            print_info("{} ({:.1f}, {:.1f}): Expected {:.9f}, Geofence {:.9f}".format(
                name, params.lat, params.lon, expected_freq, freq))
            return False
