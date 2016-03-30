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
|PBSS4130PAN|NXP - Bipolar (BJT) Single Transistor, Dual NPN, 30 V, 165 MHz, 2 W, 1 A, 180|[2291481](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2291481)|1|Q5 Dual NPN
|TDZ5V6J|NXP - Zener Single Diode, 5.6 V, 500 mW, SOD-323F, 2 %, 2 Pins, 150 Â°C|[2069517](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2069517)|1|DZ1 5V6 500mW


----

### Passives

| Part | Description | Supplier | Quantity Required | Notes
| ---  | ---         | ---       | ---               | ---
|XFL4020-222MEC|COILCRAFT - Surface Mount Power Inductor, AEC-Q200 XFL4020 Series, 2.2 ÂµH, Â± 20%, Shielded, 0.0235 ohm, 8 A|[2289216](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2289216)|1|L1 2.2µH Boost Converter XFL4020-222
|0402AF-471XJLU|COILCRAFT - Surface Mount High Frequency Inductor, 0402AF Series, 470 nH, Â± 5%, 0402 [1005 Metric], 650 MHz|[2285649](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2285649)|1|L2 470nH radio
|36501E56NJTDG|TE CONNECTIVITY - Surface Mount High Frequency Inductor, 3650 Series, 56 nH, Â± 5%, 0402 [1005 Metric], 1.76 GHz|[1265439](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1265439)|1|L3 56nH rf filter
|L-07C18NJV6T|JOHANSON TECHNOLOGY - Surface Mount High Frequency Inductor, L-07C Series, 18 nH, Â± 5%, 0402 [1005 Metric], 0.65 ohm|[1865775](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1865775)|1|L4, L5 18nH rf filter
| L6, L7 | DNP ||| si4xxx rx
|EPL2010-681MLB|COILCRAFT - Surface Mount Power Inductor, EPL2010 Series, 680 nH, Â± 20%, Shielded, 0.07 ohm, 1.94 A|[2287364](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2287364)|1|L8 SE880 EPL2010-681
|EPL2010-222MLB|COILCRAFT - Surface Mount Power Inductor, EPL2010 Series, 2.2 ÂµH, Â± 20%, Shielded, 0.222 ohm, 978 mA|[2287369](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2287369)|1|L9 SE880 EPL2010-22222
|MC00625W0402110K|MULTICOMP - Surface Mount Chip Resistor, Thick Film, MC Series, 10 kohm, 62.5 mW, Â± 1%, 50 V|[1358069](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1358069)|6|R1, R2, R3, R4, R5, R6, R7, R8 10k
|ERA2AEB433X|PANASONIC ELECTRONIC COMPONENTS - Surface Mount Chip Resistor, Metal Film, AEC-Q200 ERA Series, 43 kohm, 63 mW, Â± 0.1%, 25 V|[2324785](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2324785)|1|R9 43k 0.1%
|CPF-A-0402B100KE1|TE CONNECTIVITY - Surface Mount Chip Resistor, Thin Film, AEC-Q200 CPF-A Series, 100 kohm, 62.5 mW, Â± 0.1%, 25 V|[2483832](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2483832)|2|R10, R11 100k 0.1%
|CRG0402F100K|TE CONNECTIVITY - Surface Mount Chip Resistor, Thick Film, CRG Series, 100 kohm, 63 mW, Â± 1%, 50 V|[2331442](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2331442)|3|R12, R13, R14 100k
|MCWR04X1004FTL|MULTICOMP - Surface Mount Chip Resistor, Thick Film, MCWR Series, 1 Mohm, 62.5 mW, Â± 1%, 50 V|[2447127](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2447127)|3|R15, R16, R17 1M
|GRM1555C1H8R2CA01D|MURATA - SMD Multilayer Ceramic Capacitor, GRM Series, 8.2 pF, Â± 0.25pF, C0G / NP0, 50 V, 0402 [1005 Metric]|[1828874](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1828874)|2|C1, C3 8.2pF
|MC0402N150J500CT|MULTICOMP - SMD Multilayer Ceramic Capacitor, MC Series, 15 pF, Â± 5%, C0G / NP0, 50 V, 0402 [1005 Metric]|[1758945](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1758945)|3|C2, C4, C11 15pF
| C5, C6 | DNP ||| si4xxx rx
|MC0402N101J500CT|MULTICOMP - SMD Multilayer Ceramic Capacitor, MC Series, 100 pF, Â± 5%, C0G / NP0, 50 V, 0402 [1005 Metric]|[1758969](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1758969)|3|C8, C9, C10 100pF
|GRM155R71E103KA01D|MURATA - SMD Multilayer Ceramic Capacitor, GRM Series, 0.01 ÂµF, Â± 10%, X7R, 25 V, 0402 [1005 Metric]|[8819734](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=8819734)|1|C12 10nF LDO NR bypass
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
|20021121-00010C4LF.|AMPHENOL FCI - Board-To-Board Connector, Right Angle, Minitek127 20021121 Series, Surface Mount, Header, 10|[1865279](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1865279) [649-202112100010C4LF](http://uk.mouser.com/Search/Refine.aspx?N=1323043&Keyword=649-202112100010C4LF)|1|JTAG1
|742792731|WURTH ELEKTRONIK - FERRITE BEAD, 0.09OHM, 1.2A, 0402|[1961698](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1961698)|3|FB1, FB2, FB3
|CG0603MLC-05LE|BOURNS - ESD Protection Device, 25 V, 0603, 2 Pins, 5 V|[1828732](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1828732)|0|ESD1, ESD2
|B57861S0103F040|EPCOS - THERMISTOR, 10K, 1%, NTC, RAD|[679446](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=679446)|1|External Thermistor
| | Antenna Wire | [ebay](http://www.ebay.co.uk/itm/Ernie-Ball-Custom-Gauge-Guitar-Single-Strings-Electric-or-Acoustic-Pack-6-13-/360959212478 ) | 1 | Ernie Ball .013
| | Energizer Ultimate Lithium AAA | [datasheet](http://data.energizer.com/PDFs/l92.pdf) | 1 | ~1200mAh
| | Energizer Ultimate Lithium AA | [datasheet](http://data.energizer.com/PDFs/l91.pdf) | 0 | ~3000mAh
|FFSD-05-D-06.00-01-N|SAMTEC - RIBBON CABLE, IDC, 152.4MM, GREY, 10WAY|[1667659](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1667659)|0|10way 1.27mm
