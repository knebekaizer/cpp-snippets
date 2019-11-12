#!/usr/bin/env python

import struct
import sys

while True:
	line = sys.stdin.read(16)
	if not line: break
	data = struct.unpack("QQ", line)
	print data[0], data[1]
