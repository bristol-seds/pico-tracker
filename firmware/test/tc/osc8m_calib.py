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

class osc8m_calib_tc:
    def __init__(self):
        self.name = self.__class__.__name__
        self.iterations = 5


    def get_test(self):
        """Returns some suitable test parameters"""
        params = main.struct_osc8m_calib_tc_params()

        return params

    def is_correct(self, params, result, print_info):
        """Returns if a result is correct for the given parameters"""

        print_info(str(result))
#"OSC8M measured at %d Hz (Ctemp = %d, Cprocess = %d)"%
 #                  (result['result'], result['c_temp'], result['c_process']))

        return True
