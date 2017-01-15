#!/usr/bin/env python3
#coding: utf-8

import serial, time
arduino = serial.Serial('/dev/ttyACM0', 9600, timeout=.1)
time.sleep(1)
while True:
    data = arduino.readline()
    if data:
        print(data.decode().strip('\n'))
