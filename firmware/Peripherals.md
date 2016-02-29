## SAM D20 Peripherals usage

| Type | Peripheral | Function | Notes
| --- | --- | --- | ---
|*GLCK*|
||gclk0|main clock|fed from xosc/osc8m when awake, xosc32k/osculp32k when not awake.
||gclk1|tcxo clock|fed from xosc/osc8m. stopped when not awake.
||glck2|lf clock  |fed from xosc32k/osculp32k. always running. divided by 32 so 1024Hz nominal.
||gclk7|aprs clock|fed from gclk1, div 6 / 11
|
|*TC*||
||tc0|telemetry tick timer. 32-bit. glck1
||tc1|^^^^^
||tc2|counts cycles of tcxo. 32-bit. gclk1
||tc3|^^^^^
||tc4|unused (osc8m event source)
||tc5|telemetry pwm 16-bit glck0, ALSO aprs carrier 16-bit gclk7
|
|*EXTINT*|
||extint[5]|gps timepulse
|
|*event channels*|
||0|event source for timer 2 xosc measurement
||1|tc4 retrigger
|
|*SERCOM*||
||sercom0|spi flash
||sercom1|ublox gps
||sercom2|
||sercom3|radio|currently bitbanged as required pin layout broken in sercom

## SAM D20 Interrupts usage

| Name | Function | Priority H(0-3)L | Notes
| --- | --- | --- | ---
|TC0_IRQn|telemetry tick timer|1|latency critical for symbol timing. rate <= 1200Hz
|[GPS_SERCOM]_IRQn|gps usart rx|2|latency not so critical. rate <= 960Hz
|EIC_IRQn|timepulse|3|latency not so critical. rate = 1
|TC2_IRQn|xosc measurement done|3|latency not critical
|ADC_IRQn|adc measurement done|3|latency not critical



## Clock Layout

### At startup

```
[osc8m] --> [glck0] -> [core]
```

### Once configured

```

                           |\
          [osculp32k] --> 0| |
                           | | ------+ (22-42kHz)
lftimer -> [glck_io0] --> 1| |       |
                           |/        |
                            |        |
                      *USE_LFTIMER*  |     |\
                                     +--> 0| |
                                           | | --> [gclk0] +--> [core]
                                     +--> 1| |             |--> [tc4, wakeup, measure gclk0]
                                     |     |/
                                     |      |
                           |\        |   awake?
               [osc8m]--> 0| |       |
                           | | ------+ (8MHz/16.369MHz)
      tcxo --> [xosc] --> 1| |
                           |/
                            |
                       *USE_XOSC*




                               |\
                     |\       0| |
         [osc8m]--> 0| |       | | ---> [glck1] +--> [tc0, telemetry tick]
                     | | ---> 1| |              |--> [tc2, count tcxo]
tcxo --> [xosc] --> 1| |       |/               |--> [glck7] --> [tc5, aprs carrier] --> si_gpio1
                     |/         |               |--> [adc]
                      |      awake?             |--> [extint]
                 *USE_XOSC*                     |--> [sercoms]

```
