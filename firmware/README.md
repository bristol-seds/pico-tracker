## Bristol Longshot ##

Firmware for the Bristol Longshot tracker. Gets its GPS position home
by whatever means possible, come hell or high water.

(Actually, come low temperatures, long paths, geopolitical
restrictions, low power etc.)

## Technical Description ##

The firmware is written in C and targeted at the highly configurable
Atmel SAMD20 series of ARM Cortex M0+ micromontrollers. The code can
be built using
[GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded/).
See [README-samd20-gcc-blackmagic.md](README-samd20-gcc-blackmagic.md)
for more details.

## Developement Framework ##

Separate developement hardware exists, this has a separate MCU for
current and voltage measurement, as well controling the voltage at the
step-up input.

This is part of the development framework.

The other part of the framework is several 'development scripts' that allow

## Verification Framework ##

The verification suite ultimately allows a single command to return a
PASS/FAIL vertict on the readiness of the system to fly. There should
be no user input required during the tests - A test case that requires user
input is actually a development script.

#### REAL_HARDWARE

If this flag is set, the test scripts can presume the MCU has full
access to real flight hardware.

Otherwise the tests must presume that their target is just the MCU on
a developement board and set-up any required test harnesses as appropriate.

#### DYNAMIC_TESTS

If this flag is set, then the test cases may use dynamic data from
the internet or other sources to test the tracker's performance in
current real-world conditions. This may even include uploading the
resulting test data to habhub or similar.

Otherwise test cases must be perfectly deterministic and
repeatable. Using psudo-random sequences to generate test data is
acceptable, as long as a constant seed is used.

### LONG_TEST

If this flag is set, then the test case may make as many iterations it
feels it needs for a full and thorough test.

Otherwise test cases must take less than 1 second of real world runtime

## Sources & Licensing ##

See [LICENSE.md](LICENSE.md)
