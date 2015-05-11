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
        self.iterations = 20


    def get_test(self):
        """Returns some suitable test parameters"""
        params = main.struct_location_aprs_tc_params()
        params.input = randint(0, 10000)

        return params

    def is_correct(self, params, result, print_info):
        """Returns if a result is correct for the given parameters"""

        print_info("%d * 2 = %d"%(params.input, result['result']))

        if (params.input * 2 == result['result']):
            return True
        else:
            return False
