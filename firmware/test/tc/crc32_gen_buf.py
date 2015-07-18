#!/usr/bin/env python

# ------------------------------------------------------------------------------
# Imports
# ------------------------------------------------------------------------------

import sys
sys.path.append("./test")
import main

from random import randint
import binascii

# ------------------------------------------------------------------------------
# Test Script
# ------------------------------------------------------------------------------

class crc32_gen_buf_tc:
    def __init__(self):
        self.name = self.__class__.__name__
        self.iterations = 20

        self.test_buf_len = 32

    def get_test(self):
        """Returns some suitable test parameters"""
        params = main.struct_crc32_gen_buf_tc_params()

        """
        Assign input parameters here
        """
        # Random input buffer TODO
        for i in range(self.test_buf_len):
            params.test_buffer[i] = randint(0, 255)

        return params

    def is_correct(self, params, result, print_info):
        """Returns if a result is correct for the given parameters"""

        calculated = int(result["calculated_crc32"])
        extracted = int(result["extracted_crc32"])

        """
        Compare result and params here, decide sth.
        Can use print_info
        """
        # Generate a crc in python for this data
        py_crc = binascii.crc32(params.test_buffer) & 0xffffffff

        # Compare
        if (py_crc == calculated):
            print_info("Success! crc32 = {:#010x}".format(py_crc))
        else:
            print_info("Fail. crc32 = {:#010x}, calculated = {:#010x}".format(
                py_crc, calculated))
            return False

        # Check buffer put/get is identical
        if (calculated == extracted):
            print_info("(calcuated and extracted also identical)")
        else:
            print_info("Fail. calculated = {:#010x}, extracted = {:#010x}".format(py_crc, calculated))
            return False

        return True
