#!/usr/bin/env python

# ------------------------------------------------------------------------------
# Imports
# ------------------------------------------------------------------------------

import os
import sys

sys.path.append("./tools/verification")
from verification import *
import verification_tc

# ------------------------------------------------------------------------------
# Test Script
# ------------------------------------------------------------------------------

class times_two_tc:
    def __init__(self):
        self.name = self.__class__.__name__
        self.iterations = 20


    def get_test(self):
        """Returns some suitable test parameters"""
        params = verification_tc.struct_times_two_tc_params()
        params.input = randint(0, 10000)

        return params

    def is_correct(self, params, result):
        """Returns if a result is correct for the given parameters"""

        print_info("%d * 2 = %d"%(params.input, result['result']))

        if (params.input * 2 == result['result']):
            return True
        else:
            return False

# ------------------------------------------------------------------------------
# Run test
# ------------------------------------------------------------------------------

sys.path.append("./tools/verification/tc")

tester = samd20_test()
results = []


# Times Two
import times_two
results.append(tester.run_test_case(times_two.times_two_tc()))


# Summary
tester.print_summary(results)
# Clean Up
del tester
