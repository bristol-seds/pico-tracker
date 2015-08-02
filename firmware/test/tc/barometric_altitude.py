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
