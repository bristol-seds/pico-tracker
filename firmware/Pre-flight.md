## Updating telemetry ##

  - Set callsign in `src/main.c`

  - Update ukhas format string. `format_telemetry_string` in `src/main.c`
  - Update aprs telemetry.`encode_telemetry` in `src/aprs.c`. This same format is re-used in backlog.

  - Describe the new formats in `tools/aprs/telemetry_format.py`
  - Run `tools/aprs/equations.py` to setup new format APRS-IS

## Pre-Flight Checklist ##

- In `hw_config.h`
  - `USE_XOSC` is set. Otherwise the symbol timing frequency will be
    somewhat inaccurate.
  - `XOSC_TCXO_SHUTDOWN_EN` should be set for lowest power if required
  - `RF_TX_ENABLE` is set
  - `APRS_ENABLE` is set if APRS is to be used on the flight
  - `APRS_USE_GEOFENCE` is set
  - `TELEMETRY_ENABLE` is set
  - `TELEMETRY_USE_GEOFENCE` is set
  - `ARISS_ENABLE` is set if ARISS is to be used on the flight
  - `ARISS_USE_GEOFENCE` is set
  - `GEOFENCE_USE_PREFIX` is set
  - Check the `COLD_OUT` parameters. These prevent start-up spikes when the battery may not be viable.
  - Check the correct barometer is defined, for instance `BAROMETER_TYPE_MS5607` is set.
  - `DEBUG_USE_INTWATCHDOG` can be disabled during flight. This saves some power
  - Check interrupts are regular enough to kick the hardware
    watchdog under all conditions.

- In `watchdog.h`
  - Set the various `MAXIDLE` values to values determined during
    testing. These define how many sleep cycles are permitted in that
    state before we stop kicking the watchdog. 0xFFFF seems like a
    good conservative value, but you may want to be more agressive.

- In `aprs.h`
  - Set callsign and symbol
  - Make sure `APRS_FLIGHT_PARAMS` is defined

- In `gps_osp.c`
  - Check reinit parameters for `gps_get_data_wrapped` are reasonable.

- Check memory works and is erased
  - `make test tc=backlog_write_read` Run this more than once to check functionality of a non-blank memory
  - `make test tc=mem_erase_all`

- Check geofences are in order. Run
  - `make test tc=location_aprs`
  - `make test tc=location_telemetry`
