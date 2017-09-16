#!/usr/bin/env python3

import glob
import struct
import sys

if len(sys.argv) < 3:
    sys.exit('{} <nand-titles1.bin> <nand-titles2.bin>'.format(sys.argv[0]))

tids1 = []
tids2 = []

with open(sys.argv[1], 'rb') as t:
    tids1 = [tid[0] for tid in struct.iter_unpack('<Q', t.read())]

with open(sys.argv[2], 'rb') as t:
    tids2 = [tid[0] for tid in struct.iter_unpack('<Q', t.read())]

for tid in tids1:
    if tid not in tids2:
        print('Missing from {}: {:08X} {:08X}'.format(sys.argv[2], tid >> 32, tid & 0xFFFFFFFF))

for tid in tids2:
    if tid not in tids1:
        print('Missing from {}: {:08X} {:08X}'.format(sys.argv[1], tid >> 32, tid & 0xFFFFFFFF))
