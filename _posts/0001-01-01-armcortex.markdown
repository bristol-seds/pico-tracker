---
title: ARM Cortex M0+
tagline: Atmel SAM D20 E
linkname: armcortex
x: 49%
y: 47%
---

The board is centred around the [Atmel SAM D20][samd]
microcontroller. We use the smallest **E** variant in a QFN-32 package (_-M_)

The firmware is written in C and can be built using
[GNU Tools for ARM Embedded Processors][gnutools] See
[README-samd20-gcc-blackmagic.md][gcc-blackmagic-readme] for more
details.

From the [datasheet][datasheet] we can see there's a few RAM/ROM size
options:

|~~Variant~~|~~Flash (KBytes)~~|~~SRAM KBytes~~
|:-:|:-:|:-:
|-14|16 KBytes|2 KBytes
|-15|32 KBytes|4 KBytes
|-16|64 KBytes|8 KBytes
|_-17_|_128 KBytes_|_16 KBytes_
|-18|256 KBytes|32 KBytes
|

We've been using the _-17_ variant so far, but may upgrade to the
_-18_ variant if we need more Flash for [geofencing](#geofence) or
SRAM for [backlog](#backlog).

The SAM D series implements Generic Clocks (GCLKs) which allow us to
divide, route and disable clock signals internally as required. These
are used particularly for [APRS](#aprs).

Thanks to an error in Revision C silicon we can't configure the
correct pin layout for the SERCOM used to talk to the radio. Currently
it's just being [bit-banged][bitbang] as both Mouser and Farnell
appear to have bought large quantities of Revision C silicon. **TODO:
Fix in future board revision??**

[armcortexm0+]: http://www.arm.com/products/processors/cortex-m/cortex-m0plus.php
[samd]: http://www.atmel.com/products/microcontrollers/arm/sam-d.aspx
[datasheet]: http://www.atmel.com/Images/atmel-42129-sam-d20_datasheet.pdf
[bitbang]: https://github.com/bristol-seds/pico-tracker/blob/master/firmware/src/spi_bitbang.c#L78
[gnutools]: https://launchpad.net/gcc-arm-embedded/
[gcc-blackmagic-readme]: https://github.com/bristol-seds/pico-tracker/blob/master/firmware/README-samd20-gcc-blackmagic.md
