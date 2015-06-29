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

class mem_write_all_tc:
    def __init__(self):
        self.name = self.__class__.__name__
        self.iterations = 1


    def get_test(self):
        """Returns some suitable test parameters"""
        params = main.struct_mem_write_all_tc_params()

        """
        Assign input parameters here
        """
        for i in range(0x100):
            params.page_data[i] = randint(0, 0xff)

        return params

    def is_correct(self, params, result, print_info):
        """Returns if a result is correct for the given parameters"""

        all_good = result['all_good']
        fail_address = result['fail_address']
        fail_wrote = result['fail_wrote']
        fail_read = result['fail_read']
        """
        Compare result and params here, decide sth.
        Can use print_info
        """
        if not all_good:
            print_info("Error at index {:#x}:  {:#x} != {:#x}"
                       .format(int(fail_address), int(fail_read), int(fail_wrote)))
            return False

        print_info("All correct!")

        return True
