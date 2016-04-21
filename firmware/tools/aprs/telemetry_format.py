#
# Operations on telemetry
#
# ------------------------------------------------------------------------------

class telemetry_format_ssid:
    def __init__(self, ssid):
        if ssid is '11':
            self.flight_nr = 15
        else:
            raise ValueError("Telemetry format does not know about this ssid!")

    def flight(self):
        return self.flight_nr
    def callsign(self):
        return "UBSEDS"+str(self.flight_nr)

    #
    # Returns APRS equations for this flight
    #
    def aprs_equations(self):
        if self.flight_nr in [13,14]:
            return ["PARM.Battery,External Temperature,Internal Temperature,GPS Satellites,GPS TTFF",
                    "UNIT.Volts,Celsius,Celcius",
                    "EQNS.0,.001,0,0,.1,-273.2,0,.1,-273.2"]

        elif self.flight_nr in [15]:
            return ["PARM.Battery,Solar,External Temperature,Internal Temperature,GPS TTFF",
                    "UNIT.Volts,Volts,Celsius,Celcius",
                    "EQNS.0,.001,0,0,.001,0,0,.1,-273.2,0,.1,-273.2"]

        else:
            raise ValueError("aprs_equations does not know about this flight!")


    #
    # Returns the number of base91-encoded characters used to encode
    # telmetry on this flight. Maximum 5 analogue values
    #
    def base91_encoded_len(self):
        if self.flight_nr in [13,14]:
            return 10           # 5 analogue values
        elif self.flight_nr in [15]:
            return 10           # 5 analogue values
        else:
            raise ValueError("baseb91_encode_len does not know about this flight!")

    #
    # Decodes base91 encoded telemetry on this flight
    #
    def decode_values(self, values):
        if self.flight_nr in [13,14]:
            self.datum = {
                'battery':		(values[0] / 1000.0), # mV -> V
                'temperature_e':	(values[1] / 10.0) - 273.2, # dK -> degC
                'temperature_i':	(values[2] / 10.0) - 273.2, # dK -> degC
                'satellites':		(values[3]),
                'ttff':			(values[4])
            }
        elif self.flight_nr in [15]:
            self.datum = {
                'battery':		(values[0] / 1000.0), # mV -> V
                'solar':		(values[1] / 1000.0), # mV -> V
                'temperature_e':	(values[2] / 10.0) - 273.2, # dK -> degC
                'temperature_i':	(values[3] / 10.0) - 273.2, # dK -> degC
                'ttff':			(values[4])
            }
        else:
            raise ValueError("decode_values does not know about this flight!")

        return self.datum

    #
    # Returns a ukhas string for telmetry on this flight
    #
    def ukhas_format(self, datum=None):
        datum = datum or self.datum

        if self.flight_nr in [13,14]:
            return ("{satellites},{ttff},{battery},"
                    "{temperature_e},{temperature_i}").format(**datum)

        elif self.flight_nr in [15]:
            return ("-1,{ttff},{battery},{solar},"
                    "{temperature_e},{temperature_i}").format(**datum)
        else:
            raise ValueError("ukhas_format does not know about this flight!")

    #
    # Returns a human-readable string for telemetry on this flight
    #
    def stringify(self, datum=None):
        datum = datum or self.datum

        if self.flight_nr in [13,14]:
            return ("{battery}V {temperature_e}C {temperature_i}C "
                    "sats {satellites} ttff {ttff}").format(**datum)

        elif self.flight_nr in [15]:
            return ("{battery}V {solar}V {temperature_e}C {temperature_i}C "
                    "ttff {ttff}").format(**datum)
        else:
            raise ValueError("stringify does not know about this flight!")



#
# Telemetry format directly from a flight number
#
class telemetry_format_flight(telemetry_format_ssid):
    def __init__(self, flight_nr):
        self.flight_nr = flight_nr
