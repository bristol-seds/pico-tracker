#!/usr/bin/env python

# ------------------------------------------------------------------------------
# Verification Framework
# ------------------------------------------------------------------------------
# This script is inteneded to be run from inside gdb, with gdb already
# attached to the target - probably this is done in .gdbinit.
#
# You should be running gdb like this:
#
# arm-none-eabi-gdb -q -x tools/verification/tc/my_tc.py
#
#
# The useful output from this script appears on stderror, sorry about
# that. Hence you should supress stdout if you don't want loads of
# info from gdb.
#
# Something like this should work:
# > /dev/null arm-none-eabi-gdb -q -x tools/verfication/tc/my_tc.py
#
# ------------------------------------------------------------------------------

from __future__ import print_function

import gdb
import re
import sys
from time import *
from colorama import *


def printf(string):
        print (string, file=sys.stderr)

LINE_LENGTH = 80

class samd20_test:

        # Prints something in the centre of the line
        def print_centre(self, string):
                count = (LINE_LENGTH - len(string)) / 2
                printf ((" " * count) + string)

        # Prints a pretty header
        def print_header(self, string):
                printf (Fore.YELLOW)
                printf (("*" * LINE_LENGTH) + Fore.RESET)
                self.print_centre(string)
                printf (Fore.YELLOW + ("*" * LINE_LENGTH) + Fore.RESET)

        def print_info(self, string):
                """Prints an info line"""
                printf(Fore.CYAN + "\nINFO    " + Fore.RESET + string + "\n\n")

        #### GDB

        def __init__(self):
                self.inferior = gdb.selected_inferior()

                # Load everything into gdb and run
                gdb.execute("load")
                gdb.execute("b main")
                gdb.execute("run")

                # Stopped at the top of main. Go to tc_main
                gdb.execute("del 1")
                gdb.execute("b tc_main")
                gdb.execute("set $pc=tc_main")
                gdb.execute("c")

        def __del__(self):
                self.print_info("quit")
                gdb.execute("quit")


        def run_tc(self, tc_name, parameters):
                """Runs a test case"""

                # Write the parameters
                self.write_varible(tc_name+"_params", parameters)

                # Presuming there"s a breakpoint at the top of tc_main
                gdb.execute("set $lr=$pc")
                gdb.execute("set $pc="+tc_name)
                gdb.execute("c")

                # Test case done. Return results
                return self.read_variable(tc_name+"_results")

        #### Read / Write

        def read_variable(self, name):
                gdb.execute("p " + name, to_string=True)
                return gdb.history(0)

        def write_varible(self, name, value):
                pvar = self.read_variable(name)
                self.inferior.write_memory(pvar.address, value)
