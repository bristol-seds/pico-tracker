# Parts List

---

## Actives

| Part | Description | Supplier | Quantity Required | Notes
| ---  | ---         | ---       | ---               | ---
|SPV1040T|Voltage Regulators - Switching Regulators Hi efficiency solar battery charger|[511-SPV1040T](http://uk.mouser.com/Search/Refine.aspx?N=1323043&Keyword=511-SPV1040T)|2|MPPT U1, U2
|L6924D013TR|Battery Management Battery Charger Sys Li-Ion/Li-Poly|[511-L6924D013TR](http://uk.mouser.com/Search/Refine.aspx?N=1323043&Keyword=511-L6924D013TR)|1|Lipo Charge Controller U3
|TPS62243DRVT|TEXAS INSTRUMENTS - DC-DC CONV, BUCK, 0.3A, 2.25MHZ, SON-6|[2400648](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2400648)|2|Buck Converter DRVT = SON-6 TPS62243 U4, U5
|TPS3808G25DRVR|Supervisory Circuits Lo Quies Current Pro Delay Sup Circuit|[595-TPS3808G25DRVR](http://uk.mouser.com/Search/Refine.aspx?N=1323043&Keyword=595-TPS3808G25DRVR)|1|Vth = 2.33, DRVR = SON-6 U6
|74LVC1G04GW,125|NXP - IC, LOGIC, INVERTER, SOT-353|[1631683](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1631683)|1|Inverter U7
|BAT760|NXP - DIODE, SCHOTTKY, SOD-323|[8734593](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=8734593)|2|D3, D4


## Passives

| Part | Description | Supplier | Quantity Required | Notes
| ---  | ---         | ---       | ---               | ---
|CBC3225T100MR|TAIYO YUDEN - INDUCTOR, 1210 CASE, 10UH, 20%|[1463485](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1463485)|0|10µH Lx for MPPT. 0.133Ω esr
|SRN2512-2R2M|BOURNS - INDUCTOR, SEMI-SHLD, 2.2UH, 20%, 2.3A|[2428203](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2428203)|2|2.2µH Lx for MPPT, try this L1, L2
|SRN2512-2R2M|BOURNS - INDUCTOR, SEMI-SHLD, 2.2UH, 20%, 2.3A|[2428203](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2428203)|2|Inductutor for Buck L3, L4
|C1005X5R0J475M050BC|TDK - CAP, MLCC, X5R, 4.7UF, 6.3V, 0402|[2309027](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2309027)|4|Solar input cap 4.7uF C1, C2, C3, C4
|MC0402B102J500CT|MULTICOMP - CAP, MLCC, X7R, 1NF, 50V, 0402|[2320765](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2320765)|6|Mpp-set, vctrl, vref, vcc-sup 1nF C5, C6, C7, C8, C9, C10
|C1005X5R0J475M050BC|TDK - CAP, MLCC, X5R, 4.7UF, 6.3V, 0402|[2309027](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2309027)|2|Chg in / out 4.7µF C11, C12
|GRM155R71E473KA88D|MURATA - CAP, MLCC, X7R, 47NF, 25V, 0402|[1828868](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1828868)|1|Tprg 47nF C13
| C14 | DNP | | | Ct
|C1005X5R0J475M050BC|TDK - CAP, MLCC, X5R, 4.7UF, 6.3V, 0402|[2309027](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2309027)|2|Buck in 4.7µF C15, C16
|06036D106MAT2A|AVX - CAP, MLCC, X5R, 10UF, 6.3V, 0603|[1833804](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=1833804)|2|Buck Out 10µF 0603 C17, C18
|CRG0402F1K0|TE CONNECTIVITY - RESISTOR, FIXED, 1K0, 1%, 0402|[2331474](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2331474)|4|Mpp-set, led R1, R2, R3, R4
|MC00625W040212M20|MULTICOMP - RESISTOR, 2M2, 0.063W, 1%, 0402|[2141479](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2141479)|2|Vctrl-up R5, R6
|CRG0402F820K|TE CONNECTIVITY - RESISTOR, FIXED, 820K, 1%, 0402|[2331567](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2331567)|2|Vctrl-down R7, R8
|CRG0402F100K|TE CONNECTIVITY - RESISTOR, PRECISION, 100K, 1%, 0402|[2331442](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2331442)|2|sd-down, th-up R9, R10
|CRG0402F47K|TE CONNECTIVITY - RESISTOR, FIXED, 47K, 1%, 0402|[2331534](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2331534)|2|th-down, rprg R11, R12
|CRG0402F10K|TE CONNECTIVITY - RESISTOR, PRECISION, 10K, 1%, 0402|[2331444](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2331444)|2|Rend, batt-en-up R13, R14

## Items used in breakout verision ONLY

| Part | Description | Supplier | Quantity Required | Notes
| ---  | ---         | ---       | ---               | ---
|S1751-46R|HARWIN - TEST POINT, PCB, SMT|[2293786](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=2293786)|12|Test Point
|CTB3051/2BK|CAMDENBOSS - TERMINAL BLOCK, WIRE TO BRD, 2POS|[3882615](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=3882615)|3|Terminal Block 2 Wire
|CTB3051/3BK|CAMDENBOSS - TERMINAL BLOCK, WIRE TO BRD, 3POS|[3882627](http://uk.farnell.com/webapp/wcs/stores/servlet/Search?st=3882627)|0|Terminal Block 3 Wire
| D1, D2 | LED | | 2 | Misc
