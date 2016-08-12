#!/usr/bin/env python
# coding=utf-8

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

class repair_test_tc:
    def __init__(self):
        self.name = self.__class__.__name__

        self.index = 0
        self.addresses = [0, # Do nothing
                          0x00004000, # D1 start
                          0x00020000, # D2 start
                          0x0003C000, # EEPROM
                          ];
        self.errors_expected = [0,
                                1,
                                1,
                                0];

    def get_test(self):
        """Returns some suitable test parameters"""
        params = main.struct_repair_test_tc_params()

        if self.index >= len(self.addresses):
            return None

        params.address_to_corrupt = self.addresses[self.index]

        return params

    def is_correct(self, params, result, print_info):
        """Returns if a result is correct for the given parameters"""

        errors_corrected = int(result['errors_corrected'])
        memcmp_result = int(result['memcmp_result'])
        errors_expected = self.errors_expected[self.index]
        self.index +=1

        # Check errors
        if errors_corrected != errors_expected:
            print_info("Expected {:d} errors, actually repaired {:d} errors!"
                       .format(errors_expected, errors_corrected))
            return False

        if memcmp_result != 0:
            print_info("D1 and D2 regions differ! memcmp = {d}"
                       .format(memcmp_result))
            return False

        print_info("Case {:d} passed ✓".format(self.index))

        return True
