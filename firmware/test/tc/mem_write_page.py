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

class mem_write_page_tc:
    def __init__(self):
        self.name = self.__class__.__name__
        self.iterations = 5


    def get_test(self):
        """Returns some suitable test parameters"""
        params = main.struct_mem_write_page_tc_params()

        """
        Assign input parameters here
        """
        # There are 0x800 pages
        params.address = randint(0, 0x7ff) * 0x100

        for i in range(0x100):
            params.page[i] = randint(0, 0xff)

        return params

    def is_correct(self, params, result, print_info):
        """Returns if a result is correct for the given parameters"""

        page_read = result['page_read']
        """
        Compare result and params here, decide sth.
        Can use print_info
        """
        print_info("Address {:#06x}".format(params.address))

        for i in range(0x100):
            if not page_read[i] == params.page[i]:
                print_info("Error at index {:#x}:  {:#x} != {:#x}"
                           .format(i, int(page_read[i]), int(params.page[i])))
                return False

        print_info("All correct!")

        return True
