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

class location_telemetry_tc:
    def __init__(self):
        self.name = self.__class__.__name__

        # Load CSV file into memory
        self.locations = []
        with open("./test/tc/location_telemetry_tc_list.csv") as f:
            for line in f:
                if line[0] is not "#" and len(line) is not 0:
                    parts = [x.strip() for x in line.split(',')]
                    if len(parts) is 4:
                        self.locations.append({
                            'name': parts[0],
                            'tx_allow': float(parts[1]),
                            'lat': float(parts[2]),
                            'lon': float(parts[3])
                        })
        self.locations_index = 0


    def get_test(self):
        """Returns some suitable test parameters"""
        params = main.struct_location_telemetry_tc_params()

        if self.locations_index >= len(self.locations):
            return None

        params.lon = self.locations[self.locations_index]['lon']
        params.lat = self.locations[self.locations_index]['lat']

        return params

    def is_correct(self, params, result, print_info):
        """Returns if a result is correct for the given parameters"""

        name = self.locations[self.locations_index]['name']
        expected_tx_allow = self.locations[self.locations_index]['tx_allow']
        self.locations_index += 1

        tx_allow = result['tx_allow']

        if tx_allow == expected_tx_allow:
            print_info("{}: {}".format(name,
                                       "Permitted" if tx_allow else "Not Permitted"))
            return True
        else:
            print_info("{} ({:.1f}, {:.1f}): Expected {}, Geofence {}".format(
                name, params.lat, params.lon,
                "Permitted" if expeced_tx_allow else "Not Permitted",
                "Permitted" if tx_allow else "Not Permitted"))
            return False
