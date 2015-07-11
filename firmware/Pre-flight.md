## Pre-Flight Checklist ##

- In `hw_config.h`
  - `USE_XOSC` is set. Otherwise the symbol timing frequency will be
    somewhat inaccurate.
  - `APRS_ENABLE` is set if APRS is to be used on the flight
  - `APRS_USE_GEOFENCE` is set
  - `DEBUG_USE_INTWATCHDOG` can be disabled during flight. This saves some power
