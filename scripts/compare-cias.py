#!/usr/bin/env python3

import glob
import struct
import sys

if (len(sys.argv) < 3):
    sys.exit('{} <nand-titles.bin> <updates-folder>'.format(sys.argv[0]))

titles_nand = []
titles_cias = []

with open(sys.argv[1], 'rb') as t:
    titles_nand = [tid[0] for tid in struct.iter_unpack('<Q', t.read())]

cias = glob.iglob('{}/*.cia'.format(sys.argv[2]), recursive=True)
for cia in cias:
    with open(cia, 'rb') as f:
        f.seek(0x2C1C)
        titles_cias.append(int.from_bytes(f.read(8), 'big'))

for tid in titles_nand:
    if tid not in titles_cias:
        print('Missing from {}: {:016x}'.format(sys.argv[2], tid))

for tid in titles_cias:
    if tid not in titles_nand:
        print('Missing from NAND: {:016x}'.format(tid))
