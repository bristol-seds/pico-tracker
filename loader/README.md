## Bristol Pico Tracker ##

Bootloader for SAMD20

* Waits for battery voltage before starting boot

## Technical Description ##

The firmware is written in C and targeted at the highly configurable
Atmel SAMD20 series of ARM Cortex M0+ micromontrollers. The code can
be built using
[GNU Tools for ARM Embedded Processors](https://launchpad.net/gcc-arm-embedded/).
See [README-samd20-gcc-blackmagic.md](README-samd20-gcc-blackmagic.md)
for more details.

## Test Suite ##

The test suite is used to run test cases on real hardware. Test cases
are written mostly in python and run using a gdb that has been built
`--with-python`. It is still a work in progress but is quite handy
when debugging.

See [test/README.md](test/README.md) for more details.

## Sources & Licensing ##

See [LICENSE.md](LICENSE.md)
