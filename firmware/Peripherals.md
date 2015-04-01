## SAM D20 Peripherals usage

| Type | Peripheral | Function | Notes
| --- | --- | --- | ---
|*GLCK*|
||gclk0|main clock, internal osc8m|4 MHz
||gclk1|tcxo clock, fed from xosc

|*TC*||
||tc0|telemetry tick timer. 32-bit
||tc1|^^^^^
||tc2|counts cycles of tcxo. 32-bit
||tc3|^^^^^
||tc4|osc8m event source
||tc5|telemetry pwm 16-bit

|*EXTINT*|
||extint[5]|gps timepulse

|*event channels*|
||0|event source for timer 2 xosc measurement
||1|tc4 retrigger

|*SERCOM*||
||sercom0|spi flash
||sercom1|ublox gps
||sercom2|
||sercom3|radio|currently bitbanged as required pin layout broken in sercom

## SAM D20 Interrupts usage

| Name | Function | Priority H(0-3)L | Notes
| --- | --- | --- | ---
|TC0_IRQn|telemetry tick timer|0
|TC2_IRQn|xosc measurement done|2
|[GPS_SERCOM]_IRQn|gps usart rx|0
|EIC_IRQn|timer|1
