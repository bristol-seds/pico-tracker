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

class backlog_read_tc:
    def __init__(self):
        self.name = self.__class__.__name__

        self.iterations = 256
        self.index = 0
        self.backlog = []

    def get_test(self):
        """Returns some suitable test parameters"""
        params = main.struct_backlog_read_tc_params()

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

        if int(result['returned_null']):
            print_info("get_backlog returned null");
            return False

        # Print backlog string
        aprs_backlog_str = result['aprs_backlog_str']
        self.backlog.append(aprs_backlog_str)

        self.index = self.index + 1
        print_info("{}".format(self.index))

        if (self.index >= self.iterations): # Last iteration
            for r in range(self.iterations):
                # Playback order
                index = r
                # Physical order
                #index = sum(1<<(7-i) for i in range(8) if r>>i&1)

                # Print at index
                print_info("{:03d}: {}".format(index, self.backlog[index]))

        return True
