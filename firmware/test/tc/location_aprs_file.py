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

class location_aprs_file_tc:
    def __init__(self):
        self.name = self.__class__.__name__
        self.iterations = 500

        self.locations = []

    def teardown(self):
        # Write self.locations to json
        import json

        outfile = open('../sim/geofence/location_aprs_file.json','w')
        print >>outfile, json.dumps(self.locations, sort_keys=True, indent=2)
        outfile.close()
        return None


    def get_test(self):
        """Returns some suitable test parameters"""
        params = main.struct_location_aprs_file_tc_params()

        """
        Assign input parameters here
        """
        params.lon = float(randint(10000,13000))/1000
        params.lat = float(randint(56000,59000))/1000

        return params

    def is_correct(self, params, result, print_info):
        """Returns if a result is correct for the given parameters"""

        """
        Compare result and params here, decide sth.
        Can use print_info
        """
        freq = float(result['frequency']) / (1000*1000)

        print_info("({:.1f}, {:.1f}): {:.3f} MHz".format(
            params.lat, params.lon, freq))

        self.locations.append({
            'freq': freq,
            'lat': params.lat,
            'lon': params.lon
        })

        return True
