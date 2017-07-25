#!/usr/bin/env python

import sys, serial

p  = sys.argv[1] if len(sys.argv) > 1 else '/dev/ttyUSB0'
sp = serial.Serial(p, baudrate=9600)
while True:
  l = sp.readline().strip()
  if l: print l
