#!/usr/bin/env python

# ------------------------------------------------------------------------------
# Imports
# ------------------------------------------------------------------------------

import sys
sys.path.append("./test")
import main

from random import randint
import datetime

# ------------------------------------------------------------------------------
# Test Script
# ------------------------------------------------------------------------------

class pressure_temperature_tc:
    def __init__(self):
        self.name = self.__class__.__name__
        self.iterations = 20000000
        self.i = 0

    def get_test(self):
        """Returns some suitable test parameters"""
        params = main.struct_pressure_temperature_tc_params()

        """
        Assign input parameters here
        """

        return params

    def is_correct(self, params, result, print_info):
        """Returns if a result is correct for the given parameters"""

        """
        Compare result and params here, decide sth.
        Can use print_info
        """

        pressure = result['pressure']
        temperature = result['temperature']

        print_info("Pressure: {:.1f} Pa, Temperature: {:.2f} degC   {}".format(
            float(pressure), float(temperature), datetime.datetime.now().time()))


        return True
