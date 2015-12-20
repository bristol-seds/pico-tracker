#### Hardware Revisions

Most hardware that has been built has a version number

|Version|Description|Exists/Existed|Flights used on
|----|----|----|----
|v0.96.0|Original AAA Battery. Flown with modifications to connect tcxo to mcu.|x|UBSEDS3-8?
|v0.981.0|Solar Power SPV1040. Flown with modifications simplify power circuit.|x|UBSEDS9
|v0.981.1|Minor fixes|
|v0.982.0|AAA Battery. Never fully populated, used for lab testing.|x|
|v0.982.1|Minor fixes|
|v0.984.0|First attempt at internal balloon board|
|v0.985.0|Solar Power/Primary AAA Lithium|x|UBSEDS10-11
|v0.985.1|Added DNP/0Ω between solar and MPPT set for use when only a battery is connected|
|v0.985.2|BMP180 -> MS5607 Pressure / Temperature Sensor|
|v0.985.3|Smaller holes for external thermistor|
|v0.986.0|Low Power with side mounting GPS chip antenna|
|v0.986.1|Low Power. Add 10k i2c pull ups|
|v0.986.1|Low Power. Rearranged matching network|
|v0.986.3|Low Power. Added L0'-C0' tank in paralell. Might work, should test|
|v0.986.4|Low Power. Added LIS2D accelerometer. Optional|
|v0.986.5|Low Power. Added TS3006 low power lf clock|
|v0.986.6|Low Power. Added LDO for GPS, LF TCXO for GPS|
|v0.986.7|Low Power. Changed low power lf clock, added vreg on breakout for optional power testing|
|v0.986.8|Low Power. Changed memory pads, GPS LDO and specified SAMD20E18A-MN with more flash and higher grade packaging|x|
|v0.986.9|Low Power. 80x80mm Breakout|
