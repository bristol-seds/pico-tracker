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

class backlog_write_read_tc:
    def __init__(self):
        self.name = self.__class__.__name__

        self.record_not_get = 1
        self.index = 0
        self.list_len = randint(160, 240) # How many records to write
        self.roffset = randint(10, 99) * 10*1000
        self.list_thing = range(self.roffset, self.roffset+self.list_len)

        self.iterations = 256 + self.list_len

    def get_test(self):
        """Returns some suitable test parameters"""
        params = main.struct_backlog_write_read_tc_params()

        """
        Assign input parameters here
        """
        params.record_not_get = self.record_not_get
        params.epoch_write = (self.index + self.roffset)

        return params

    def is_correct(self, params, result, print_info):
        """Returns if a result is correct for the given parameters"""

        """
        Compare result and params here, decide sth.
        Can use print_info
        """

        if self.index % 10 == 0:
            if self.record_not_get:
                print_info("RECORD index {}".format(self.index))
            else:
                print_info("GET index {}".format(self.index))

        # Okay we need to check we get everything back here
        if (self.record_not_get == 0):
            epoch_read = result['epoch_read']
            print_info("Read epoch {}".format(epoch_read))
            if epoch_read in self.list_thing:
                # Delete by value
                self.list_thing.remove(epoch_read)
        else:
            wptr = result["wptr"]
            print_info("wptr {}".format(wptr-1));




        self.index += 1
        if self.record_not_get and self.index == self.list_len: # Finished writing
            self.record_not_get = 0 # Start reading
            self.index = 0
        elif self.index == 256: # Should be done now
            if len(self.list_thing) == 0:
                print_info("All {} backlogs replayed correctly".format(self.list_len))
            else:
                print_info("Some backlogs not replayed:")
                print_info(str(self.list_thing))
                return False


        return True
