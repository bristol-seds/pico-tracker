## SAM D20 Peripherals usage

| Type | Peripheral | Function | Notes
| --- | --- | --- | ---
|*GLCK*|
||gclk0|main|2MHz??
|*TC*||
||tc0|
||tc1|
||tc3|there was the txco on glck_io[3], but the pin voltages didn't work
||tc5|pwm
|*SERCOM*||
||sercom0|spi flash
||sercom1|ublox gps
||sercom2|
||sercom3|radio|currently bitbanged as required pin layout broken in sercom
