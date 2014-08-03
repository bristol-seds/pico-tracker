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

LINE_LENGTH = 80

##### Public Print utilities

def printf(string):
        """All writes go to stderr"""
        print (string, file=sys.stderr)

def print_info(string):
        """Prints an info line"""
        printf("")
        printf(Fore.CYAN + "INFO    " + Fore.RESET + string)

def print_good(string):
        """Prints an good line"""
        printf(Fore.GREEN + "GOOD    " + Fore.RESET + string)

def print_error(string):
        """Prints an error line"""
        printf(Fore.RED + "ERROR    " + Fore.RESET + string)

##### Tester

class samd20_test:

        def print_centre(self, string):
                """Prints something in the centre of the line"""
                count = (LINE_LENGTH - len(string)) / 2
                printf ((" " * count) + string)

        def print_header_line(self):
                """Prints a yellow line. Yo"""
                printf (Fore.YELLOW + ("*" * LINE_LENGTH) + Fore.RESET)

        def print_header(self, string):
                """Prints a pretty header"""
                printf ("")
                self.print_header_line()
                self.print_centre(string)
                self.print_header_line()

        def print_pass(self, tc_name, time):
                """Nice green pass notice"""
                offset = (LINE_LENGTH / 2) - len(tc_name)

                printf("")
                printf(Fore.GREEN + "    " + tc_name + " - PASS" \
                       + (" " * offset) + "CLOCKS = " + str(time) + Fore.RESET)

        def print_fail(self, tc_name, time):
                """Evil red pass notice"""
                offset = (LINE_LENGTH / 2) - len(tc_name)

                printf("")
                printf(Fore.RED + "  p  " + tc_name + "- FAIL" \
                       + (" " * offset) + "CLOCKS = " + str(time) + Fore.RESET)

        def print_summary(self, results):
                passes = 0
                for result in results:
                        (passed) = result
                        if passed:
                                passes += 1


                self.print_header("SUMMARY - %d%% PASS"%(100*passes/len(results)))

                for result in results:
                        (passed, name, time) = result
                        if passed:
                                self.print_pass(name, time)
                        else:
                                self.print_fail(name, time)

                self.print_header("")
        #### GDB

        def __init__(self):
                self.inferior = gdb.selected_inferior()

                # Pagination is not helpful here
                gdb.execute("set pagination off")

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
                print_info("quit")
                gdb.execute("quit")


        def hw_run_tc(self, tc_name, parameters):
                """Runs a test case on hardware"""

                # If we're stopped where we'd expect
                if self.read_variable("($pc == tc_main + 4)"):

                        # Write the parameters
                        self.write_varible(tc_name+"_params", parameters)

                        # Presuming there"s a breakpoint at the top of tc_main
                        gdb.execute("set tc_ptr="+tc_name+"+1")
                        gdb.execute("c")

                        # Test case done. Return results
                        return self.read_variable(tc_name+"_results")
                else:
                        return None

        def hw_get_last_time(self):
                """Returns the number of clocks the last test case took"""
                return 1


        #### Read / Write

        def read_variable(self, name):
                gdb.execute("p " + name, to_string=True)
                return gdb.history(0)

        def write_varible(self, name, value):
                pvar = self.read_variable(name)
                self.inferior.write_memory(pvar.address, value)

        #### Test Case


        def run_test_case(self, test_case):
                tc_name = test_case.__class__.__name__
                self.print_header(tc_name)
                fail = False
                ttime = 0

                if hasattr(test_case, 'iterations'):
                        for i in range(test_case.iterations):
                                params = test_case.get_test()
                                result = self.hw_run_tc(tc_name, params)
                                ttime += self.hw_get_last_time()

                                if result:
                                        if not test_case.is_correct(params, result):
                                                fail = True
                                                break
                                else: # No result, Failure
                                        fail = True
                else:
                        params = test_case.get_test()

                        while (params):
                                result = self.hw_run_tc(tc_name, params)
                                ttime += self.hw_get_last_time()

                                if result:
                                        if not test_case.is_correct(params, result):
                                                fail = True
                                                break
                                else: # No result, Failure
                                        fail = True

                                params = test_case.get_test()

                if not fail:
                        self.print_pass(tc_name, ttime)
                else:
                        self.print_fail(tc_name, ttime)

                # Return data tuple
                return (not fail, tc_name, ttime)
