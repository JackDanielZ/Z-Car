EESchema Schematic File Version 2
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
LIBS:kicad_components
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L RPi0_GPIO U?
U 1 1 592B4325
P 4050 3000
F 0 "U?" H 4050 3000 60  0001 C CNN
F 1 "RPi0_GPIO" H 4250 3000 60  0000 C CNN
F 2 "" H 4050 3000 60  0001 C CNN
F 3 "" H 4050 3000 60  0001 C CNN
	1    4050 3000
	1    0    0    -1  
$EndComp
$Comp
L L293D U?
U 1 1 592B43BA
P 7250 4000
F 0 "U?" H 7250 4000 60  0001 C CNN
F 1 "L293D" V 7250 3450 60  0000 C CNN
F 2 "" H 7250 4000 60  0001 C CNN
F 3 "" H 7250 4000 60  0001 C CNN
	1    7250 4000
	0    1    1    0   
$EndComp
$Comp
L Battery_Cell 2x3.7V/9800mAh
U 1 1 592B46C9
P 6600 2650
F 0 "2x3.7V/9800mAh" H 6700 2750 50  0000 L CNN
F 1 "Battery_Cell" H 6700 2650 50  0000 L CNN
F 2 "" V 6600 2710 50  0001 C CNN
F 3 "" V 6600 2710 50  0001 C CNN
	1    6600 2650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7100 3700 7500 3700
Wire Wire Line
	7500 3700 7500 4300
Wire Wire Line
	7500 4300 7100 4300
Wire Wire Line
	6400 4300 6400 4800
Wire Wire Line
	6400 4800 7100 4800
Wire Wire Line
	7100 4300 7100 4800
Wire Wire Line
	7100 4800 7100 4950
Wire Wire Line
	4700 3150 5650 3150
Wire Wire Line
	5650 3150 5650 4950
Wire Wire Line
	5650 4950 7100 4950
Connection ~ 7100 4800
Wire Wire Line
	6800 4300 6800 4900
Wire Wire Line
	6800 4900 5700 4900
Wire Wire Line
	5700 4900 5700 3350
Wire Wire Line
	5700 3350 4700 3350
Wire Wire Line
	4700 3850 6000 3850
Wire Wire Line
	6000 3850 6000 3200
Wire Wire Line
	6000 3200 6500 3200
Wire Wire Line
	6500 3200 6500 3700
Wire Wire Line
	4700 3950 6100 3950
Wire Wire Line
	6100 3950 6100 3100
Wire Wire Line
	6100 3100 7000 3100
Wire Wire Line
	7000 3100 7000 3700
Wire Wire Line
	4700 5050 6500 5050
Wire Wire Line
	6500 5050 6500 4300
Wire Wire Line
	4700 5150 7000 5150
Wire Wire Line
	7000 5150 7000 4300
Wire Wire Line
	6700 2650 6700 3700
Wire Wire Line
	6400 2650 6400 3700
$Comp
L Motor Wheels_Motor
U 1 1 592B4D1A
P 7800 5000
F 0 "Wheels_Motor" H 7800 5000 60  0001 C CNN
F 1 "Motor" H 7800 4800 60  0000 C CNN
F 2 "" H 7800 5000 60  0001 C CNN
F 3 "" H 7800 5000 60  0001 C CNN
	1    7800 5000
	1    0    0    -1  
$EndComp
$Comp
L Motor Wheels_Motor
U 1 1 592B4D46
P 7800 2250
F 0 "Wheels_Motor" H 7800 2250 60  0001 C CNN
F 1 "Motor" H 7800 2050 60  0000 C CNN
F 2 "" H 7800 2250 60  0001 C CNN
F 3 "" H 7800 2250 60  0001 C CNN
	1    7800 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	6600 3700 6600 2900
Wire Wire Line
	6600 2900 7400 2900
Wire Wire Line
	7400 2900 7400 2750
Wire Wire Line
	6900 3700 6900 3000
Wire Wire Line
	6900 3000 8200 3000
Wire Wire Line
	8200 3000 8200 2750
Wire Wire Line
	6600 4300 6600 5500
Wire Wire Line
	6600 5500 7400 5500
Wire Wire Line
	6900 4300 6900 5050
Wire Wire Line
	6900 5050 8200 5050
Wire Wire Line
	8200 5050 8200 5500
$EndSCHEMATC
