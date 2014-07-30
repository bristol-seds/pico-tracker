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
    def __init__(self, tester):
        self.name = self.__class__.__name__

        tester.print_header(self.__class__.__name__)

        params = verification_tc.struct_times_two_tc_params()
        params.input = 4

        result = tester.run_tc(self.name, params)

        tester.print_info(str(result))

# ------------------------------------------------------------------------------
# Run test
# ------------------------------------------------------------------------------

tester = samd20_test()
times_two_tc(tester)
del tester
