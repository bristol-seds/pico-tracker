## SAM D20 Peripherals usage

| Type | Peripheral | Function | Notes
| --- | --- | --- | ---
|*GLCK*|
||gclk0|main clock, internal osc8m|4 MHz
||gclk1|tcxo clock, fed from xosc

|*TC*||
||tc0|telemetry tick timer (has timepulse input)
||tc1|^^^^^
||tc2|counts cycles of tcxo. 32-bit
||tc3|^^^^^
||tc4|osc8m event source
||tc5|pwm 8-bit

|*event channels*|
||0|event source for timer 2 xosc measurement
||1|tc4 retrigger

|*SERCOM*||
||sercom0|spi flash
||sercom1|ublox gps
||sercom2|
||sercom3|radio|currently bitbanged as required pin layout broken in sercom

## SAM D20 Interrupts usage

| Name | Function | Notes
| --- | --- | --- | ---
|TC0_IRQn|
|TC2_IRQn|xosc measurement done|
