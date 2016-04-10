# Parts List

low-power-solar

----
### Active

| Part | Description | Supplier | Quantity Required | Notes
| ---  | ---         | ---       | ---               | ---
|ATSAMD20E18A-MN|ATMEL - 32 Bit Microcontroller, SAM D Series, ARM Cortex-M0+, 48 MHz, 256 KB, 32 KB, 32 Pins, QFN|[2460535](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2460535)|1|U1
|SI4461-C2A-GM|SILICON LABS - RF TRANSCEIVER, 0.142-1.05GHZ, HQFN-20|[2462636](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2462636)|1|U2 Si4461-C
| Telit SE880 | GPS | [Sequoia](http://www.sequoia.co.uk/product.php?id=1761) [Digikey](http://www.digikey.co.uk/product-detail/en/SE880GPSR22R003/943-1028-1-ND/5050348) | 1 | U3
|SST25WF040BT-40I/NP|MICROCHIP - FLASH, 4MBIT, 40MHZ, USON-8|[2414758](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2414758)|1|U4 memory 4Mbit
|X1G0041310008 TG-5006CJ-17H 16.369MHZ|EPSON - CRYSTAL, TCXO, TG-5006CJ-17H, 16.369 MHZ|[2405790](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2405790)|1|U5 TCXO Si4xxx and SE880
|TPS73118DBVT|TEXAS INSTRUMENTS - Fixed LDO Voltage Regulator, 1.7V to 5.5V, 30mV Dropout, 1.8Vout, 150mAout, SOT-23-5|[1135388](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1135388)|1|U6 LDO for TXCO
|ADM6823WYRJZ-RL7|ANALOG DEVICES - Supervisory Circuit, Manual Reset, Active-Low Reset, 1V-5.5Vin, SOT-23-5|[2457340](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2457340)|1|U7 watchdog Vth = 1.67V, t_WDmin = 1.12s
|SN74LVC1G32DCKR|TEXAS INSTRUMENTS - OR Gate, LVC Family, 1 Gate, 2 Input, 32 mA, 1.65V to 5.5V, SC-70-5|[1470880](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1470880)|1|U8 or-gate, watchdog subsystem
|TPS62243DRVT|TEXAS INSTRUMENTS - DC-DC CONV, BUCK, 0.3A, 2.25MHZ, SON-6|[2400648](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2400648) [595-TPS62243DRVT](http://uk.mouser.com/Search/Refine.aspx?N=1323043&Keyword=595-TPS62243DRVT)|1|U9 Buck Converter DRVT = SON-6 TPS62243
| SIT1552AI-JF-DCC-32.768 | 32.768kHz TCXO, 1.5 - 3.63 V, 1.5 x 0.8mm CSP  | [Mouser](http://www.mouser.co.uk/ProductDetail/SiTime/SIT1552AI-JE-DCC-32768E/) [Digikey](http://www.digikey.co.uk/product-detail/en/SIT1552AI-JF-DCC-32.768D/1473-1242-1-ND/5035517) | 2 | U10, U11 lf clock, gps lf tcxo clock
|LTC4070EMS8E#PBF|LINEAR TECHNOLOGY - BATTERY CHARGER, LIION/POLY, 8MSOP|[1839141](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1839141)|1|U12 Li-ion battery limit LTC4070
|ADP1614ACPZ-1.3-R7|ANALOG DEVICES - DC-DC Switching Boost (Step Up) Regulator, Adjustable, 2.5V-5.5Vin, 3.6V-20Vout, 4Aout, LFCSP-10|[2191713](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2191713)|1|U13 ADP1614ACPZ-1.3 7.5V Boost Reg
|TPS71533DCKR|TEXAS INSTRUMENTS - Fixed LDO Voltage Regulator, 3V to 24V, 415mV Dropout, 3.3Vout, 50mAout, SC-70-5|[1135383](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1135383)|2|U14, U15 TPS71533DCKR Fixed LDO 3.3V, 24Vin max, 50mA
|74LVC1G04GW,125|NXP - Inverter Gate, LVC Family, Ultra High Speed, 1 Input, 32 mA, 1.65V to 5.5V, TSSOP-5|[1631683](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1631683)|3|U16, U17, U18 Inverter
|AS213-92LF|SKYWORKS SOLUTIONS - IC, SWITCH, RF, SPDT, 0.1-3.0GHZ|[1753766](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1753766)|1|U19 SPDT RF Switch 1.8V
| SKY13373-460LF | 0.1-6.0 GHz SP3T Switch  | [Mouser](http://www.mouser.co.uk/ProductDetail/Skyworks-Solutions-Inc/SKY13373-460LF/) [Digikey](http://www.digikey.com/product-detail/en/skyworks-solutions-inc/SKY13373-460LF/863-1768-1-ND/) | 2 | U20 SP3T RF switch
|NX2301P|NXP - MOSFET Transistor, P Channel, -2 A, -20 V, 0.1 ohm, -4.5 V, -750 mV|[1894738](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1894738)|2|Q1, Q2 NX2301 P-type MOSFET
|BSS84AK|NXP - MOSFET Transistor, P Channel, -180 mA, -50 V, 4.5 ohm, -10 V, -1.6 V|[1972673](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1972673)|2|Q3, Q4 BSS84 P-channel MOSFET
|PBSS4130PAN|NXP - Bipolar (BJT) Single Transistor, Dual NPN, 30 V, 165 MHz, 2 W, 1 A, 180|[2291481](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2291481)|1|Q5 Dual NPN
|PD84001|STMICROELECTRONICS - RF FET Transistor, 18 V, 1.5 A, 6 W, 1 GHz, SOT-89|[2341737](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2341737)|1|Q6 RF amplifier MOSFET
|RB161M-20TR|ROHM - Small Signal Schottky Diode, Single, 25 V, 1 A, 350 mV, 30 A, 125 Â°C|[1525484](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1525484)|5|D1, D2, D3, D4, D5 Small signal schottky RB161M
|MBR0520LT1G|ON SEMICONDUCTOR - Schottky Rectifier, Single, 20 V, 500 mA, SOD-123, 2 Pins, 385 mV|[9556915](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=9556915)|1|D6 MBR0520LT1G Schottky diode 500mA for 7.5v boost
|TDZ5V6J|NXP - Zener Single Diode, 5.6 V, 500 mW, SOD-323F, 2 %, 2 Pins, 150 Â°C|[2069517](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2069517)|1|DZ1 5V6 500mW
----

### Inductors

| Part | Description | Supplier | Quantity Required | Notes
| ---  | ---         | ---       | ---               | ---
|0402AF-471XJLU|COILCRAFT - Surface Mount High Frequency Inductor, 0402AF Series, 470 nH, Â± 5%, 0402 [1005 Metric], 650 MHz|[2285649](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2285649)|1|LC 470nH radio
|36501E56NJTDG|TE CONNECTIVITY - Surface Mount High Frequency Inductor, 3650 Series, 56 nH, Â± 5%, 0402 [1005 Metric], 1.76 GHz|[1265439](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1265439)|1|L0 56nH rf filter
|L-07C18NJV6T|JOHANSON TECHNOLOGY - Surface Mount High Frequency Inductor, L-07C Series, 18 nH, Â± 5%, 0402 [1005 Metric], 0.65 ohm|[1865775](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1865775)|2|L1, L2 18nH rf filter
| L3, L4 | DNP ||| si4xxx rx
|EPL2010-681MLB|COILCRAFT - Surface Mount Power Inductor, EPL2010 Series, 680 nH, Â± 20%, Shielded, 0.07 ohm, 1.94 A|[2287364](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2287364)|1|L5 SE880 EPL2010-681
|SRN2512-2R2M|BOURNS - Surface Mount Power Inductor, SRN2512 Series, 2.2 ÂµH, Â± 20%, Semishielded, 0.102 ohm, 2.3 A|[2428203](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2428203)|1|L6 Inductor for Buck 2.2µH
|XAL4030-472MEC|COILCRAFT - Surface Mount Power Inductor, AEC-Q200 XAL40xx Series, 4.7 ÂµH, Â± 20%, Shielded, 0.0441 ohm, 5.1 A|[2289056](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2289056)|1|L7 Lx for 7.5v boost XAL4030 4.7µF
|0805LS-111XJLB|COILCRAFT - Surface Mount High Frequency Inductor, 0805LS Series, 110 nH, Â± 5%, 0805 [2012 Metric], 1.2 GHz|[2286542](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2286542)|1|L8 0805LS-111X for amplifier 100nH
|744761133C|WURTH ELEKTRONIK - Surface Mount High Frequency Inductor, WE-KI Series, 33 nH, Â± 5%, 0603 [1608 Metric], 2.3 GHz|[1748772](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1748772)|2|L9, L10 33nH 0603


## Resistors

| Part | Description | Supplier | Quantity Required | Notes
| ---  | ---         | ---       | ---               | ---
|RC0402JR-070RL|YAGEO (PHYCOMP) - Surface Mount Chip Resistor, Thick Film, RC Series, 0 ohm, 62.5 mW, Â± 5%, 50 V, 0402 [1005 Metric]|[9232516](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=9232516)|1|0R
|MC00625W040214R7|MULTICOMP - Surface Mount Chip Resistor, Thick Film, MC Series, 4.7 ohm, 62.5 mW, Â± 1%, 50 V|[1357987](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1357987)|1|4.7R
|MC0063W0603122R|MULTICOMP - Surface Mount Chip Resistor, Thick Film, MC Series, 22 ohm, 63 mW, Â± 1%, 50 V, 0603 [1608 Metric]|[9330844](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=9330844)|2|22R 0603
|CRG0402F330R|TE CONNECTIVITY - Surface Mount Chip Resistor, Thick Film, CRG Series, 330 ohm, 63 mW, Â± 1%, 50 V, 0402 [1005 Metric]|[2331511](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2331511)|1|330R
|CRG0402F1K0|TE CONNECTIVITY - Surface Mount Chip Resistor, Thick Film, CRG Series, 1 kohm, 63 mW, Â± 1%, 50 V, 0402 [1005 Metric]|[2331474](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2331474)|4|1k
|MCMR04X472 JTL|MULTICOMP - Surface Mount Chip Resistor, Ceramic, MCMR Series, 4.7 kohm, 62.5 mW, Â± 5%, 50 V|[2073069](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2073069)|1|4.7k
|MC00625W0402110K|MULTICOMP - Surface Mount Chip Resistor, Thick Film, MC Series, 10 kohm, 62.5 mW, Â± 1%, 50 V|[1358069](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1358069)|9|10k
|MCMR04X203 JTL|MULTICOMP - Surface Mount Chip Resistor, Ceramic, MCMR Series, 20 kohm, 62.5 mW, Â± 5%, 50 V, 0402 [1005 Metric]|[2072757](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2072757)|1|20k
|MC00625W0402133K|MULTICOMP - Surface Mount Chip Resistor, Thick Film, MC Series, 33 kohm, 62.5 mW, Â± 1%, 50 V|[1358083](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1358083)|2|33k
|MCWR04X7502FTL|MULTICOMP - Surface Mount Chip Resistor, Thick Film, MCWR Series, 75 kohm, 62.5 mW, Â± 1%, 50 V|[2447215](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2447215)|1|75k
|CPF-A-0402B100KE1|TE CONNECTIVITY - Surface Mount Chip Resistor, Thin Film, AEC-Q200 CPF-A Series, 100 kohm, 62.5 mW, Â± 0.1%, 25 V|[2483832](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2483832)|2|100k 0.1%
|CRG0402F100K|TE CONNECTIVITY - Surface Mount Chip Resistor, Thick Film, CRG Series, 100 kohm, 63 mW, Â± 1%, 50 V|[2331442](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2331442)|3|100k
|MCWR04X1004FTL|MULTICOMP - Surface Mount Chip Resistor, Thick Film, MCWR Series, 1 Mohm, 62.5 mW, Â± 1%, 50 V|[2447127](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2447127)|3|1M

## Capacitors

| Part | Description | Supplier | Quantity Required | Notes
| ---  | ---         | ---       | ---               | ---

|GRM1555C1H8R2CA01D|MURATA - SMD Multilayer Ceramic Capacitor, GRM Series, 8.2 pF, Â± 0.25pF, C0G / NP0, 50 V, 0402 [1005 Metric]|[1828874](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1828874)|2|8.2pF
|MC0402N150J500CT|MULTICOMP - SMD Multilayer Ceramic Capacitor, MC Series, 15 pF, Â± 5%, C0G / NP0, 50 V, 0402 [1005 Metric]|[1758945](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1758945)|2|15pF
|MC0402N270J500CT|MULTICOMP - SMD Multilayer Ceramic Capacitor, MC Series, 27 pF, Â± 5%, C0G / NP0, 50 V, 0402 [1005 Metric]|[1758953](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1758953)|1|27pF
|0402N390K250CT|WALSIN - SMD Multilayer Ceramic Capacitor, General Purpose, 39 pF, Â± 10%, C0G / NP0, 25 V|[2524782](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2524782)|1|39pF
|MC0402N560J500CT|MULTICOMP - SMD Multilayer Ceramic Capacitor, MC Series, 56 pF, Â± 5%, C0G / NP0, 50 V, 0402 [1005 Metric]|[1758961](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1758961)|2|56pF
|MC0402N101J500CT|MULTICOMP - SMD Multilayer Ceramic Capacitor, MC Series, 100 pF, Â± 5%, C0G / NP0, 50 V, 0402 [1005 Metric]|[1758969](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1758969)|3|100pF
|GRM155R71E103KA01D|MURATA - SMD Multilayer Ceramic Capacitor, GRM Series, 0.01 ÂµF, Â± 10%, X7R, 25 V, 0402 [1005 Metric]|[8819734](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=8819734)|6|10nF
|0805B103K101CT|WALSIN - SMD Multilayer Ceramic Capacitor, 0.01 ÂµF, Â± 10%, X7R, 100 V, 0805 [2012 Metric]|[2496937](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2496937)|1|1nF 0805
|MC0402B183K160CT|MULTICOMP - SMD Multilayer Ceramic Capacitor, MC Series, 0.018 ÂµF, Â± 10%, X7R, 16 V, 0402 [1005 Metric]|[1758889](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1758889)|1|18nF
|MC0402X104K100CT|MULTICOMP - SMD Multilayer Ceramic Capacitor, MC Series, 0.1 ÂµF, Â± 10%, X5R, 10 V, 0402 [1005 Metric]|[1759380](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1759380)|1|47nF
|MC0402X104K100CT|MULTICOMP - SMD Multilayer Ceramic Capacitor, MC Series, 0.1 ÂµF, Â± 10%, X5R, 10 V, 0402 [1005 Metric]|[1759380](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1759380)|14|100nF
|GRM155R61E105KA12D|MURATA - SMD Multilayer Ceramic Capacitor, GRM Series, 1 ÂµF, Â± 10%, X5R, 25 V, 0402 [1005 Metric]|[2218855](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2218855)|4|1µF
|GRM155R60J225ME15D|MURATA - SMD Multilayer Ceramic Capacitor, GRM Series, 2.2 ÂµF, Â± 20%, X5R, 6.3 V, 0402 [1005 Metric]|[2362088](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2362088)|2|2.2µF
|C1005X5R0J475M050BC|TDK - SMD Multilayer Ceramic Capacitor, C Series, 4.7 ÂµF, Â± 20%, X5R, 6.3 V, 0402 [1005 Metric]|[2309027](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2309027)|2|4.7uF
|06036D106MAT2A|AVX - SMD Multilayer Ceramic Capacitor, 10 ÂµF, Â± 20%, X5R, 6.3 V, 0603 [1608 Metric]|[1833804](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1833804)|3|10µF 0603
|GRM31CR60J107ME39L.|MURATA - SMD Multilayer Ceramic Capacitor, GRM Series, 100 ÂµF, Â± 20%, X5R, 6.3 V, 1206 [3216 Metric]|[1735535](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1735535)|2|100µF 1206



----

### Misc

| Part | Description | Supplier | Quantity Required | Notes
| ---  | ---         | ---       | ---               | ---
|20021121-00010C4LF.|AMPHENOL FCI - Board-To-Board Connector, Right Angle, Minitek127 20021121 Series, Surface Mount, Header, 10|[1865279](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1865279) [649-202112100010C4LF](http://uk.mouser.com/Search/Refine.aspx?N=1323043&Keyword=649-202112100010C4LF)|1|JTAG1
|742792731|WURTH ELEKTRONIK - FERRITE BEAD, 0.09OHM, 1.2A, 0402|[1961698](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1961698)|2|FB1, FB2
|742792641|WURTH ELEKTRONIK - FERRITE, BEAD, 0603, 0.15OHM, 2A|[1635705](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1635705)|1|FB3 (0603)
|CG0603MLC-05LE|BOURNS - ESD Protection Device, 25 V, 0603, 2 Pins, 5 V|[1828732](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1828732)|0|ESD1, ESD2
|B57861S0103F040|EPCOS - THERMISTOR, 10K, 1%, NTC, RAD|[679446](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=679446)|1|External Thermistor
| 1575AT43A40 | GPS Antenna | [Proto-pic](http://proto-pic.co.uk/antenna-gps-chip-scale/) | 1 | A1 1.575GHz
| | Antenna Wire | [ebay](http://www.ebay.co.uk/itm/Ernie-Ball-Custom-Gauge-Guitar-Single-Strings-Electric-or-Acoustic-Pack-6-13-/360959212478 ) | 1 | Ernie Ball .013
| | Energizer Ultimate Lithium AAA | [datasheet](http://data.energizer.com/PDFs/l92.pdf) | 1 | ~1200mAh
| | Energizer Ultimate Lithium AA | [datasheet](http://data.energizer.com/PDFs/l91.pdf) | 0 | ~3000mAh
|FFSD-05-D-06.00-01-N|SAMTEC - RIBBON CABLE, IDC, 152.4MM, GREY, 10WAY|[1667659](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1667659)|0|10way 1.27mm
