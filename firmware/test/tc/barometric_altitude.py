#!/usr/bin/env python

# ------------------------------------------------------------------------------
# Imports
# ------------------------------------------------------------------------------

import sys
sys.path.append("./test")
import main

from random import normalvariate

# ------------------------------------------------------------------------------
# Test Script
# ------------------------------------------------------------------------------

class barometric_altitude_tc:
    def __init__(self):
        self.name = self.__class__.__name__
        self.iterations = 25

        # Maybe use test data from Wolfram Alpha
        self.test_pressures = [102500, 101300, 89880, 70120, 41110, 22700,
                               12110, 5529, 2549, 1197, 575, 287]
        self.test_altitudes = [-100, 0, 1000, 3000, 7000, 11000, 15000,
                               20000, 25000, 30000, 35000, 40000]


    def get_test(self):
        """Returns some suitable test parameters"""
        params = main.struct_barometric_altitude_tc_params()

        """
        Assign input parameters here
        """
        # Random barometeric pressure
        params.pressure = abs(normalvariate(0, 80*1000))

        return params

    def is_correct(self, params, result, print_info):
        """Returns if a result is correct for the given parameters"""

        """
        Compare result and params here, decide sth.
        Can use print_info
        """

        altitude = result['altitude']

        print_info("Pressure: {:.1f} Pa, Altitude: {:.2f} meters".format(
            params.pressure, float(altitude)))
        print_info("")

        return True
