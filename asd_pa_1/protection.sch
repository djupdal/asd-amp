EESchema Schematic File Version 2
LIBS:poweramp-rescue
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:amp
LIBS:poweramp-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 2
Title "AsD PA 1 Protection Circuit"
Date "1 oct 2014"
Rev ""
Comp "Asbjørn Djupdal"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L BC556 Q16
U 1 1 542BF567
P 5300 2650
F 0 "Q16" H 5300 2501 40  0000 R CNN
F 1 "BC556" H 5300 2800 40  0000 R CNN
F 2 "Housings_TO-92:TO-92-Free-inline-narrow-oval" H 5200 2752 29  0001 C CNN
F 3 "" H 5300 2650 60  0001 C CNN
	1    5300 2650
	1    0    0    1   
$EndComp
$Comp
L BC546 Q18
U 1 1 542BF55D
P 7000 2450
F 0 "Q18" H 7000 2301 40  0000 R CNN
F 1 "BC546" H 7000 2600 40  0000 R CNN
F 2 "Housings_TO-92:TO-92-Free-inline-narrow-oval" H 6900 2552 29  0001 C CNN
F 3 "" H 7000 2450 60  0001 C CNN
	1    7000 2450
	1    0    0    -1  
$EndComp
$Comp
L BC546 Q17
U 1 1 542BF55B
P 6050 1800
F 0 "Q17" H 6050 1651 40  0000 R CNN
F 1 "BC546" H 6050 1950 40  0000 R CNN
F 2 "Housings_TO-92:TO-92-Free-inline-narrow-oval" H 5950 1902 29  0001 C CNN
F 3 "" H 6050 1800 60  0001 C CNN
	1    6050 1800
	1    0    0    -1  
$EndComp
$Comp
L BC546 Q15
U 1 1 542BF556
P 5300 2100
F 0 "Q15" H 5300 1951 40  0000 R CNN
F 1 "BC546" H 5300 2250 40  0000 R CNN
F 2 "Housings_TO-92:TO-92-Free-inline-narrow-oval" H 5200 2202 29  0001 C CNN
F 3 "" H 5300 2100 60  0001 C CNN
	1    5300 2100
	1    0    0    -1  
$EndComp
$Comp
L DIODE D10
U 1 1 5412EC1B
P 9950 5400
F 0 "D10" H 9950 5600 70  0000 C CNN
F 1 "1N4004" H 9900 5200 70  0000 C CNN
F 2 "Diodes_ThroughHole:Diode_DO-41_SOD81_Horizontal_RM10" H 9950 5400 60  0001 C CNN
F 3 "" H 9950 5400 60  0001 C CNN
	1    9950 5400
	0    -1   -1   0   
$EndComp
$Comp
L R-RESCUE-poweramp R27
U 1 1 5412EA52
P 6850 5700
AR Path="/5412EA52" Ref="R27"  Part="1" 
AR Path="/5411DDF6/5412EA52" Ref="R27"  Part="1" 
F 0 "R27" V 6930 5700 50  0000 C CNN
F 1 "100" V 6850 5700 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" H 6850 5700 60  0001 C CNN
F 3 "" H 6850 5700 60  0001 C CNN
	1    6850 5700
	0    -1   -1   0   
$EndComp
$Comp
L C-RESCUE-poweramp C12
U 1 1 5412EA48
P 6200 5700
AR Path="/5412EA48" Ref="C12"  Part="1" 
AR Path="/5411DDF6/5412EA48" Ref="C12"  Part="1" 
F 0 "C12" V 6300 5850 50  0000 C CNN
F 1 "10u" V 6300 5550 50  0000 C CNN
F 2 "amp:47u_NP" H 6200 5700 60  0001 C CNN
F 3 "" H 6200 5700 60  0001 C CNN
F 4 "NP" V 6100 5550 60  0000 C CNN "Note"
	1    6200 5700
	0    -1   -1   0   
$EndComp
$Comp
L R-RESCUE-poweramp R26
U 1 1 5416CCBE
P 6150 3150
AR Path="/5416CCBE" Ref="R26"  Part="1" 
AR Path="/5411DDF6/5416CCBE" Ref="R26"  Part="1" 
F 0 "R26" V 6230 3150 50  0000 C CNN
F 1 "100k" V 6150 3150 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" H 6150 3150 60  0001 C CNN
F 3 "" H 6150 3150 60  0001 C CNN
	1    6150 3150
	-1   0    0    1   
$EndComp
$Comp
L DIODE D6
U 1 1 54160B69
P 4650 1700
F 0 "D6" H 4650 1900 70  0000 C CNN
F 1 "1N4148" H 4600 1500 70  0000 C CNN
F 2 "Diodes_ThroughHole:Diode_DO-35_SOD27_Horizontal_RM10" H 4650 1700 60  0001 C CNN
F 3 "" H 4650 1700 60  0001 C CNN
	1    4650 1700
	0    1    1    0   
$EndComp
$Comp
L DIODE D7
U 1 1 54160B51
P 4650 3050
F 0 "D7" H 4650 3250 70  0000 C CNN
F 1 "1N4148" H 4600 2850 70  0000 C CNN
F 2 "Diodes_ThroughHole:Diode_DO-35_SOD27_Horizontal_RM10" H 4650 3050 60  0001 C CNN
F 3 "" H 4650 3050 60  0001 C CNN
	1    4650 3050
	0    1    1    0   
$EndComp
$Comp
L R-RESCUE-poweramp R25
U 1 1 54160B0A
P 5400 3200
AR Path="/54160B0A" Ref="R25"  Part="1" 
AR Path="/5411DDF6/54160B0A" Ref="R25"  Part="1" 
F 0 "R25" V 5480 3200 50  0000 C CNN
F 1 "4.7k" V 5400 3200 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" H 5400 3200 60  0001 C CNN
F 3 "" H 5400 3200 60  0001 C CNN
	1    5400 3200
	-1   0    0    1   
$EndComp
$Comp
L R-RESCUE-poweramp R24
U 1 1 54160B02
P 5400 1400
AR Path="/54160B02" Ref="R24"  Part="1" 
AR Path="/5411DDF6/54160B02" Ref="R24"  Part="1" 
F 0 "R24" V 5480 1400 50  0000 C CNN
F 1 "100k" V 5400 1400 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" H 5400 1400 60  0001 C CNN
F 3 "" H 5400 1400 60  0001 C CNN
	1    5400 1400
	-1   0    0    1   
$EndComp
$Comp
L R-RESCUE-poweramp R22
U 1 1 54160A74
P 3400 2400
AR Path="/54160A74" Ref="R22"  Part="1" 
AR Path="/5411DDF6/54160A74" Ref="R22"  Part="1" 
F 0 "R22" V 3480 2400 50  0000 C CNN
F 1 "47k" V 3400 2400 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" H 3400 2400 60  0001 C CNN
F 3 "" H 3400 2400 60  0001 C CNN
	1    3400 2400
	0    -1   -1   0   
$EndComp
$Comp
L C-RESCUE-poweramp C10
U 1 1 54160A67
P 3800 3250
AR Path="/54160A67" Ref="C10"  Part="1" 
AR Path="/5411DDF6/54160A67" Ref="C10"  Part="1" 
F 0 "C10" V 3900 3400 50  0000 C CNN
F 1 "47u" V 3900 3100 50  0000 C CNN
F 2 "amp:47u_NP" H 3800 3250 60  0001 C CNN
F 3 "" H 3800 3250 60  0001 C CNN
F 4 "NP" V 4000 3100 60  0000 C CNN "Note"
	1    3800 3250
	1    0    0    -1  
$EndComp
$Comp
L DIODE D5
U 1 1 54160A4C
P 4300 2650
F 0 "D5" H 4300 2850 70  0000 C CNN
F 1 "1N4148" H 4250 2450 70  0000 C CNN
F 2 "Diodes_ThroughHole:Diode_DO-35_SOD27_Horizontal_RM10" H 4300 2650 60  0001 C CNN
F 3 "" H 4300 2650 60  0001 C CNN
	1    4300 2650
	-1   0    0    1   
$EndComp
$Comp
L DIODE D4
U 1 1 54160A3F
P 4300 2100
F 0 "D4" H 4300 2300 70  0000 C CNN
F 1 "1N4148" H 4250 1900 70  0000 C CNN
F 2 "Diodes_ThroughHole:Diode_DO-35_SOD27_Horizontal_RM10" H 4300 2100 60  0001 C CNN
F 3 "" H 4300 2100 60  0001 C CNN
	1    4300 2100
	1    0    0    -1  
$EndComp
$Comp
L DIODE D8
U 1 1 5416080C
P 5700 3950
F 0 "D8" H 5700 4150 70  0000 C CNN
F 1 "1N4004" H 5650 3750 70  0000 C CNN
F 2 "Diodes_ThroughHole:Diode_DO-41_SOD81_Horizontal_RM10" H 5700 3950 60  0001 C CNN
F 3 "" H 5700 3950 60  0001 C CNN
	1    5700 3950
	0    -1   -1   0   
$EndComp
$Comp
L R-RESCUE-poweramp R28
U 1 1 5415C5F2
P 7100 3150
AR Path="/5415C5F2" Ref="R28"  Part="1" 
AR Path="/5411DDF6/5415C5F2" Ref="R28"  Part="1" 
F 0 "R28" V 7180 3150 50  0000 C CNN
F 1 "270" V 7100 3150 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" H 7100 3150 60  0001 C CNN
F 3 "" H 7100 3150 60  0001 C CNN
	1    7100 3150
	-1   0    0    1   
$EndComp
Text HLabel 1000 1050 0    60   Input ~ 0
Mute
$Comp
L DIODE D9
U 1 1 5412EC14
P 9950 4500
F 0 "D9" H 9950 4700 70  0000 C CNN
F 1 "1N4004" H 9900 4300 70  0000 C CNN
F 2 "Diodes_ThroughHole:Diode_DO-41_SOD81_Horizontal_RM10" H 9950 4500 60  0001 C CNN
F 3 "" H 9950 4500 60  0001 C CNN
	1    9950 4500
	0    -1   -1   0   
$EndComp
Text HLabel 1000 6400 0    60   Input ~ 0
Vee
Text HLabel 1000 6650 0    60   Input ~ 0
GndZ
$Comp
L C-RESCUE-poweramp C11
U 1 1 5412E2BA
P 5350 5500
AR Path="/5412E2BA" Ref="C11"  Part="1" 
AR Path="/5411DDF6/5412E2BA" Ref="C11"  Part="1" 
F 0 "C11" V 5450 5650 50  0000 C CNN
F 1 "100n" V 5450 5350 50  0000 C CNN
F 2 "Capacitors_ThroughHole:Capacitor7x2.5RM5" H 5350 5500 60  0001 C CNN
F 3 "" H 5350 5500 60  0001 C CNN
	1    5350 5500
	1    0    0    -1  
$EndComp
$Comp
L R-RESCUE-poweramp R23
U 1 1 5412E2B9
P 5350 6050
AR Path="/5412E2B9" Ref="R23"  Part="1" 
AR Path="/5411DDF6/5412E2B9" Ref="R23"  Part="1" 
F 0 "R23" V 5250 6050 50  0000 C CNN
F 1 "10" V 5350 6050 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM15mm" H 5350 6050 60  0001 C CNN
F 3 "" H 5350 6050 60  0001 C CNN
F 4 "4W" V 5450 6050 60  0000 C CNN "Note"
	1    5350 6050
	1    0    0    -1  
$EndComp
$Comp
L INDUCTOR L1
U 1 1 5412E2B8
P 8650 4950
F 0 "L1" V 8775 4950 70  0000 C CNN
F 1 "2u" V 8600 4950 70  0000 C CNN
F 2 "amp:Inductor_AirCore_2uH" H 8650 4950 60  0001 C CNN
F 3 "" H 8650 4950 60  0001 C CNN
	1    8650 4950
	0    -1   1    0   
$EndComp
$Comp
L R-RESCUE-poweramp R29
U 1 1 5412E2B7
P 8650 4650
AR Path="/5412E2B7" Ref="R29"  Part="1" 
AR Path="/5411DDF6/5412E2B7" Ref="R29"  Part="1" 
F 0 "R29" V 8730 4650 50  0000 C CNN
F 1 "10" V 8650 4650 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM15mm" H 8650 4650 60  0001 C CNN
F 3 "" H 8650 4650 60  0001 C CNN
F 4 "4W" V 8550 4650 60  0000 C CNN "Note"
	1    8650 4650
	0    -1   -1   0   
$EndComp
$Comp
L RELAY_SPDT XK1
U 1 1 5412E2B4
P 6500 4900
F 0 "XK1" H 6450 5100 70  0000 C CNN
F 1 "RELAY_SPDT" H 6450 4400 70  0001 C CNN
F 2 "amp:Omron_G2R_SPDT" H 6500 4900 60  0001 C CNN
F 3 "" H 6500 4900 60  0001 C CNN
F 4 "Omron G2R-1 24V" H 6450 4400 60  0000 C CNN "Note"
	1    6500 4900
	-1   0    0    -1  
$EndComp
Text HLabel 10350 4950 2    60   Input ~ 0
Speaker
Text HLabel 1000 4250 0    60   Input ~ 0
Gnd
Text HLabel 1000 850  0    60   Input ~ 0
Vcc
Text HLabel 1000 5050 0    60   Input ~ 0
AmpOut
Wire Wire Line
	7250 4950 7250 5700
Wire Wire Line
	7250 5700 7100 5700
Wire Wire Line
	6400 5700 6600 5700
Wire Wire Line
	5800 5700 6000 5700
Wire Wire Line
	5800 5700 5800 5050
Connection ~ 5800 5050
Connection ~ 7250 4950
Wire Wire Line
	5700 4150 5700 4850
Wire Wire Line
	6150 3400 6150 4250
Connection ~ 6150 850 
Connection ~ 6150 4250
Connection ~ 2750 4250
Wire Wire Line
	2750 4250 2750 1300
Wire Wire Line
	2750 1300 4650 1300
Wire Wire Line
	4650 1300 4650 1500
Connection ~ 4650 2650
Wire Wire Line
	4650 2850 4650 2650
Connection ~ 5400 1800
Wire Wire Line
	5400 2850 5400 2950
Wire Wire Line
	5400 1650 5400 1900
Wire Wire Line
	4500 2650 5100 2650
Wire Wire Line
	4100 2100 4000 2100
Wire Wire Line
	4000 2100 4000 2650
Connection ~ 3800 2400
Wire Wire Line
	3650 2400 4000 2400
Wire Wire Line
	3800 2400 3800 3050
Connection ~ 7100 3650
Wire Wire Line
	5700 3750 5700 3650
Connection ~ 7100 850 
Wire Wire Line
	7100 2250 7100 850 
Wire Wire Line
	7100 2900 7100 2650
Wire Wire Line
	9950 850  9950 4300
Wire Wire Line
	1000 850  9950 850 
Wire Wire Line
	1000 6400 9950 6400
Wire Wire Line
	9950 6400 9950 5600
Connection ~ 9950 4950
Wire Wire Line
	1000 5050 6100 5050
Wire Wire Line
	5350 6300 5350 6650
Wire Wire Line
	5350 5700 5350 5800
Connection ~ 7900 4950
Wire Wire Line
	7900 4950 7900 4650
Wire Wire Line
	7900 4650 8400 4650
Wire Wire Line
	8900 4650 9400 4650
Connection ~ 9400 4950
Wire Wire Line
	9400 4650 9400 4950
Wire Wire Line
	5350 5050 5350 5300
Connection ~ 5350 5050
Wire Wire Line
	1000 4250 7000 4250
Wire Wire Line
	9950 4700 9950 5200
Wire Wire Line
	5350 6650 1000 6650
Connection ~ 5700 4250
Wire Wire Line
	3150 2400 3000 2400
Wire Wire Line
	3000 2400 3000 5050
Connection ~ 3000 5050
Wire Wire Line
	3800 3450 3800 4250
Connection ~ 3800 4250
Wire Wire Line
	4000 2650 4100 2650
Connection ~ 4000 2400
Wire Wire Line
	4500 2100 5100 2100
Wire Wire Line
	5400 2300 5400 2450
Wire Wire Line
	5400 1150 5400 850 
Connection ~ 5400 850 
Wire Wire Line
	4950 6400 4950 3600
Connection ~ 4950 6400
Wire Wire Line
	4650 4250 4650 3250
Connection ~ 4650 4250
Wire Wire Line
	4650 1900 4650 2100
Connection ~ 4650 2100
Wire Wire Line
	4950 3600 5400 3600
Wire Wire Line
	5400 3600 5400 3450
Wire Wire Line
	1000 1050 5700 1050
Wire Wire Line
	5700 4850 6100 4850
Wire Wire Line
	5400 1800 5850 1800
Wire Wire Line
	5700 1050 5700 1800
Connection ~ 5700 1800
Wire Wire Line
	6150 2000 6150 2900
Wire Wire Line
	6800 2450 6150 2450
Connection ~ 6150 2450
Wire Wire Line
	6150 850  6150 1600
Wire Wire Line
	5700 3650 7100 3650
Wire Wire Line
	6900 4950 8350 4950
Wire Wire Line
	8950 4950 10350 4950
Wire Wire Line
	6900 5250 7000 5250
Wire Wire Line
	7000 5250 7000 4250
Wire Wire Line
	7100 5150 6900 5150
Wire Wire Line
	7100 3400 7100 5150
$EndSCHEMATC
