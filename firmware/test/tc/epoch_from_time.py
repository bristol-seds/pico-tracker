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

class epoch_from_time_tc:
    def __init__(self):
        self.name = self.__class__.__name__
        self.iterations = 20


    def get_test(self):
        """Returns some suitable test parameters"""
        params = main.struct_epoch_from_time_tc_params()

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

        epoch = int(result['epoch'])
        print_info("calculated epoch is {}".format(epoch))

        if epoch is 1467312480:
            return True
        else:
            print_info("correct epoch is {}".format(1467312480))
            return False
