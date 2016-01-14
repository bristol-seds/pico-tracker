## Pre-Flight Checklist ##

- In `hw_config.h`
  - `USE_XOSC` is set. Otherwise the symbol timing frequency will be
    somewhat inaccurate.
  - `RF_TX_ENABLE` is set
  - `APRS_ENABLE` is set if APRS is to be used on the flight
  - `APRS_USE_GEOFENCE` is set
  - `TELEMETRY_USE_GEOFENCE` is set
  - `DEBUG_USE_INTWATCHDOG` can be disabled during flight. This saves some power
  - Check interrupts are regular enough to kick the hardware
    watchdog.

- In `watchdog.h`
  - Set the various `MAXIDLE` values to values determined during
    testing. These define how many sleep cycles are permitted in that
    state before we stop kicking the watchdog. 0xFFFF seems like a
    good conservative value, but you may want to be more agressive.

- In `aprs.h`
  - Set callsign and symbol
  - Make sure `APRS_FLIGHT_PARAMS` is defined

- In `main.c`
  - Set callsign
