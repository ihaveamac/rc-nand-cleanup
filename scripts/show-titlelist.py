#!/usr/bin/env python3

import glob
import struct
import sys

if len(sys.argv) < 2:
    sys.exit('{} <nand-titles.bin>'.format(sys.argv[0]))

with open(sys.argv[1], 'rb') as t:
    for tid in struct.iter_unpack('<Q', t.read()):
        print('{:08X} {:08X}'.format(tid[0] >> 32, tid[0] & 0xFFFFFFFF))
