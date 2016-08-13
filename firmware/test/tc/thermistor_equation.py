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

class thermistor_equation_tc:
    def __init__(self):
        self.name = self.__class__.__name__

        # Params
        self.series_resistor = 10**5
        self.thermistor_nominal = 10**4 # Nominal thermistor resistance at 25 degC
        self.tolerance = 1 # Pass/fail error tolerance

        # Resistance of thermistor at -55 to 100 degrees every 5 degrees, taken from datasheet
        self.resistances = [96.3, 67.01, 47.17, 33.65, 24.26, 17.7, 13.04, 9.707, 7.293,
                            5.533, 4.232, 3.365, 2.539, 1.99, 1.571, 1.249, 1.000,
                            0.8057, 0.6531, 0.5327, 0.4369, 0.3603, 0.2986, 0.2488,
                            0.2083, 0.1752, 0.1481, 0.1258, 0.1072, 0.09117, 0.07885, 0.068];

        self.iterations = len(self.resistances)
        self.index = 0


    def get_test(self):
        """Returns some suitable test parameters"""
        params = main.struct_thermistor_equation_tc_params()

        """
        Assign input parameters here
        """
        i = self.index

        # calculate expected ADC value for the current temperature
        thermistor_resistance = (self.resistances[i] * self.thermistor_nominal)
        params.value = (self.series_resistor/
                        (thermistor_resistance + self.series_resistor))

        return params

    def is_correct(self, params, result, print_info):
        """Returns if a result is correct for the given parameters"""
        i = self.index
        self.index = self.index + 1

        expected = -55+(i*5)
        calculated = float(result["temperature"])

        """
        Compare result and params here, decide sth.
        Can use print_info
        """

        error = expected - calculated

        print_info("expected: {}degC calculated: {}degC (error: {})".format(expected, calculated, error))

        if (abs(error) > self.tolerance):
            print_info("ERROR: {}".format(error))
            return False

        return True
