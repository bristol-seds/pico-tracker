# Parts List

----
### Active

| Part | Description | Supplier | Quantity Required | Notes
| ---  | ---         | ---       | ---               | ---
|ATSAMD20E18A-MN|ATMEL - 32 Bit Microcontroller, SAM D Series, ARM Cortex-M0+, 48 MHz, 256 KB, 32 KB, 32, QFN|[2460535](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2460535)|1|U1
|SI4468-A2A-IM|SILICON LABS - RF TRANSCEIVER, 0.142-1.05GHZ, HQFN-20|[2462639](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2462639)|1|U2
| Telit SE880 | GPS | [Sequoia](http://www.sequoia.co.uk/product.php?id=1761) [Digikey](http://www.digikey.co.uk/product-detail/en/SE880GPSR22R003/943-1028-1-ND/5050348) | 1 | U3
|SST25WF040BT-40I/NP|MICROCHIP - FLASH, 4MBIT, 40MHZ, USON-8|[2414758](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2414758)|1|U4 memory 4Mbit
|X1G0041310008 TG-5006CJ-17H 16.369MHZ|EPSON - CRYSTAL, TCXO, TG-5006CJ-17H, 16.369 MHZ|[2405790](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2405790)|2|U5, U6 TCXO Si4xxx, SE880
|TPS3123J18DBVT|TEXAS INSTRUMENTS - IC, 1.62V SUPPLY MONITOR SOT-23-5|[1755607](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1755607)|1|U7 watchdog
|SN74LVC1G32DCKR|TEXAS INSTRUMENTS - IC, SINGLE 2 INPUT OR GATE, SMD|[1470880](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1470880)|1|U8 or-gate, watchdog subsystem
|MS560702BA03-50|MEASUREMENT SPECIALTIES - Pressure Sensor, Barometric, 10 mbar, 1200 mbar, 1.8 V, 3.6 V|[2362660](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2362660)|1|U9 MS5607 Pressure / Temperature sensor
|ADP1607ACPZN-R7|ANALOG DEVICES - DC-DC Switching Boost Step Up Regulator, Adjustable, 800mV-3.3Vin, 1.8V-3.3Vout, 1Aout, LFCSP-6|[2254940](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2254940)|1|U10 Boost Reg ADP1607
|TPS73118DBVT|TEXAS INSTRUMENTS - Fixed LDO Voltage Regulator, 1.7V to 5.5V, 30mV Dropout, 1.8Vout, 150mAout, SOT-23-5|[1135388](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1135388)|1|U11 LDO for TXCO
|MIC94310-GYM5 TR|MICREL SEMICONDUCTOR - Fixed LDO Voltage Regulator, 1.8V to 3.6V, 40mV Dropout, 1.8Vout, 200mAout, SOT-23-5|[2451765](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2451765)|1|U12 LDO for GPS MIC94310-GYM5
| SIT1552AI-JF-DCC-32.768 | 32.768kHz TCXO, 1.5 - 3.63 V, 1.5 x 0.8mm CSP  | [Mouser](http://www.mouser.co.uk/ProductDetail/SiTime/SIT1552AI-JE-DCC-32768E/?qs=sGAEpiMZZMt8oz%2fHeiymANsH3mGAL6UqdcmVNtl5EmRPF6ciZe04Kw%3d%3d) [Digikey](http://www.digikey.co.uk/product-detail/en/SIT1552AI-JF-DCC-32.768D/1473-1242-1-ND/5035517) | 2 | U13, U14 lf clock, gps lf tcxo clock
|LIS2DH|STMICROELECTRONICS - MEMS Accelerometer, 3-Axis, Digital, X, Y, Z, Â± 2g, Â± 4g, Â± 8g, Â± 16g, 1.71 V, 3.6 V, LGA|[2462671](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2462671)|1|U15 3-axis accel I2C. Optional
----

### Passives

| Part | Description | Supplier | Quantity Required | Notes
| ---  | ---         | ---       | ---               | ---
|XFL4020-222MEC|COILCRAFT - Surface Mount Power Inductor, AEC-Q200 XFL4020 Series, 2.2 ÂµH, Â± 20%, Shielded, 0.0235 ohm, 8 A|[2289216](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2289216)|1|L1 2.2µH Boost Converter XFL4020-222
|0402AF-221XJLU|COILCRAFT - Surface Mount High Frequency Inductor, 0402AF Series, 220 nH, Â± 5%, 0402 [1005 Metric], 1.15 GHz|[2285640](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2285640)|1|L2 220nH radio
|LQG15HN56NJ02D|MURATA - INDUCTOR, 0402 CASE, 56NHÂ±5%|[1343091](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1343091)|1|L3 56nH rf filter
|L-07C18NJV6T|JOHANSON TECHNOLOGY - Surface Mount High Frequency Inductor, L-07C Series, 18 nH, Â± 5%, 0402 [1005 Metric], 0.65 ohm|[1865775](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1865775)|1|L4, L5 18nH rf filter
| L6, L7 | DNP ||| si4xxx rx
|EPL2010-681MLB|COILCRAFT - Surface Mount Power Inductor, EPL2010 Series, 680 nH, Â± 20%, Shielded, 0.07 ohm, 1.94 A|[2287364](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2287364)|1|L8 SE880 EPL2010-681
|EPL2010-222MLB|COILCRAFT - Surface Mount Power Inductor, EPL2010 Series, 2.2 ÂµH, Â± 20%, Shielded, 0.222 ohm, 978 mA|[2287369](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2287369)|1|L9 SE880 EPL2010-22222
|MC00625W0402110K|MULTICOMP - Surface Mount Chip Resistor, Thick Film, MC Series, 10 kohm, 62.5 mW, Â± 1%, 50 V|[1358069](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1358069)|6|R1, R2, R3, R4, R10, R11 10k
|CRG0402F100K|TE CONNECTIVITY - Surface Mount Chip Resistor, Thick Film, CRG Series, 100 kohm, 63 mW, Â± 1%, 50 V|[2331442](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2331442)|2|R5, R6 100k
|CPF-A-0402B100KE1|TE CONNECTIVITY - Surface Mount Chip Resistor, Thin Film, AEC-Q200 CPF-A Series, 100 kohm, 62.5 mW, Â± 0.1%, 25 V|[2483832](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2483832)|2|R7, R9 100k 0.1%
|ERA2AEB433X|PANASONIC ELECTRONIC COMPONENTS - Surface Mount Chip Resistor, Metal Film, AEC-Q200 ERA Series, 43 kohm, 63 mW, Â± 0.1%, 25 V|[2324785](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2324785)|1|R8 43k 0.1%
|RC0402JR-070RL|YAGEO (PHYCOMP) - Surface Mount Chip Resistor, Thick Film, RC Series, 0 ohm, 62.5 mW, Â± 5%, 50 V, 0402 [1005 Metric]|[9232516](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=9232516)|1|R12 0R
|GRM1555C1H8R2CA01D|MURATA - SMD Multilayer Ceramic Capacitor, GRM Series, 8.2 pF, Â± 0.25pF, C0G / NP0, 50 V, 0402 [1005 Metric]|[1828874](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1828874)|2|C1, C3 8.2pF
|MC0402N150J500CT|MULTICOMP - SMD Multilayer Ceramic Capacitor, MC Series, 15 pF, Â± 5%, C0G / NP0, 50 V, 0402 [1005 Metric]|[1758945](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1758945)|2|C2, C4 15pF
| C5, C6 | DNP ||| si4xxx rx
|MC0402N101J500CT|MULTICOMP - SMD Multilayer Ceramic Capacitor, MC Series, 100 pF, Â± 5%, C0G / NP0, 50 V, 0402 [1005 Metric]|[1758969](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1758969)|3|C8, C9, C10 100pF
|MC0402B102K500CT|MULTICOMP - SMD Multilayer Ceramic Capacitor, MC Series, 1000 pF, Â± 10%, X7R, 50 V, 0402 [1005 Metric]|[1758991](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1758991)|1|C11 1nF
|GRM155R71E103KA01D|MURATA - SMD Multilayer Ceramic Capacitor, GRM Series, 0.01 ÂµF, Â± 10%, X7R, 25 V, 0402 [1005 Metric]|[8819734](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=8819734)|1|C11 10nF LDO NR bypass
| C12 | DNP ||| LDO NC
|MC0402X104K100CT|MULTICOMP - SMD Multilayer Ceramic Capacitor, MC Series, 0.1 ÂµF, Â± 10%, X5R, 10 V, 0402 [1005 Metric]|[1759380](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1759380)|9|C13, C14, C15, C16, C17, C18, C19, C20, C21 100nF
|GRM155R61E105KA12D|MURATA - SMD Multilayer Ceramic Capacitor, GRM Series, 1 ÂµF, Â± 10%, X5R, 25 V, 0402 [1005 Metric]|[2218855](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2218855)|3|C22, C23, C24, C28 1µF
|GRM155R60J225ME15D|MURATA - SMD Multilayer Ceramic Capacitor, GRM Series, 2.2 ÂµF, Â± 20%, X5R, 6.3 V, 0402 [1005 Metric]|[2362088](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2362088)|1|C25 Radio Bypass 2.2µF
|C1005X5R0J475M050BC|TDK - SMD Multilayer Ceramic Capacitor, C Series, 4.7 ÂµF, Â± 20%, X5R, 6.3 V, 0402 [1005 Metric]|[2309027](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2309027)|1|C26 MS5607 Bypass 4.7µF
|MC0402B102K500CT|MULTICOMP - SMD Multilayer Ceramic Capacitor, MC Series, 1000 pF, Â± 10%, X7R, 50 V, 0402 [1005 Metric]|[1758991](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1758991)|1|C27 1nF
|06036D106MAT2A|AVX - SMD Multilayer Ceramic Capacitor, 10 ÂµF, Â± 20%, X5R, 6.3 V, 0603 [1608 Metric]|[1833804](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1833804)|3|C29, C30, C31 Boost in, out 10µF 0603
|GRM31CR60J107ME39L.|MURATA - SMD Multilayer Ceramic Capacitor, GRM Series, 100 ÂµF, Â± 20%, X5R, 6.3 V, 1206 [3216 Metric]|[1735535](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1735535)|1|C32 100µF optional
| 1575AT43A40 | GPS Antenna | [Proto-pic](http://proto-pic.co.uk/antenna-gps-chip-scale/) | 1 | A1 1.575GHz

----

### Misc

| Part | Description | Supplier | Quantity Required | Notes
| ---  | ---         | ---       | ---               | ---
|20021121-00010C4LF.|FCI - Board-To-Board Connector, Right Angle, Minitek127 20021121 Series, Surface Mount, Header, 10|[1865279](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1865279) [649-202112100010C4LF](http://uk.mouser.com/Search/Refine.aspx?N=1323043&Keyword=649-202112100010C4LF)|1|JTAG1
|742792731|WURTH ELEKTRONIK - FERRITE BEAD, 0.09OHM, 1.2A, 0402|[1961698](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1961698)|2|FB1, FB2
|CG0603MLC-05LE|BOURNS - ESD Protection Device, 25 V, 5 V, 0603, 2|[1828732](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1828732)|0|ESD1, ESD2
|B57861S0103F040|EPCOS - THERMISTOR, 10K, 1%, NTC, RAD|[679446](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=679446)|1|External Thermistor
| | Antenna Wire | [ebay](http://www.ebay.co.uk/itm/Ernie-Ball-Custom-Gauge-Guitar-Single-Strings-Electric-or-Acoustic-Pack-6-13-/360959212478 ) | 1 | Ernie Ball .013
| | Energizer Ultimate Lithium AAA | [datasheet](http://data.energizer.com/PDFs/l92.pdf) | 1 | ~1200mAh
| | Energizer Ultimate Lithium AA | [datasheet](http://data.energizer.com/PDFs/l91.pdf) | 0 | ~3000mAh
|FFSD-05-D-06.00-01-N|SAMTEC - RIBBON CABLE, IDC, 152.4MM, GREY, 10WAY|[1667659](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1667659)|0|10way 1.27mm