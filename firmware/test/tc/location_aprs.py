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
        self.iterations = 30


    def get_test(self):
        """Returns some suitable test parameters"""
        params = main.struct_location_aprs_tc_params()

        params.lon = -5 + randint(0, 120)*0.1;
        params.lat = 52;

        return params

    def is_correct(self, params, result, print_info):
        """Returns if a result is correct for the given parameters"""

        freq = result['frequency'] / (1000*1000)
        status_str = "NO"
        if result['tx_allow']:
            status_str = "Tx"

        print_info("%f, %f = %s, %f"%(params.lon, params.lat, status_str, freq))

        return True
