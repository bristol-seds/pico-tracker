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

class mem_erase_all_tc:
    def __init__(self):
        self.name = self.__class__.__name__
        self.iterations = 1


    def get_test(self):
        """Returns some suitable test parameters"""
        params = main.struct_mem_erase_all_tc_params()

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
        print_info("Memory erased")

        return True
