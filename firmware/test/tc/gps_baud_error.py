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

class gps_baud_error_tc:
    def __init__(self):
        self.name = self.__class__.__name__
        self.iterations = 1


    def get_test(self):
        """Returns some suitable test parameters"""
        params = main.struct_gps_baud_error_tc_params()

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
        calc = int(result["calculated_baud_milli"])/1000.0
        perip = result["peripheral_clock"]
        intended = result["intended_baud"]
        register = result["register_value"]
        error = float(abs(calc-intended))

        print_info("""{:.3f} Hz on a {} MHz clock
        (Intended {} Hz, register = {:#06x}, error = {:.3f} Hz)""".format(
            calc, perip, intended, int(register), error))

        if (error < 1):
            return True
        else:
            return False
