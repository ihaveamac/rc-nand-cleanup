#!/usr/bin/env python3

import csv
import struct
import sys

if len(sys.argv) < 3:
    sys.exit('{} <nand-titles.bin> <ninupdates-titles.csv>'.format(sys.argv[0]))

titles_nand = []
titles_ninupdates = {}

with open(sys.argv[1], 'rb') as t:
    titles_nand = [tid[0] for tid in struct.iter_unpack('<Q', t.read())]

with open(sys.argv[2], 'r') as c:
    r = csv.reader(c)
    for row in r:
        try:
            tid = int(row[0], 16)
            if tid not in titles_ninupdates:
                titles_ninupdates[tid] = []
            titles_ninupdates[tid].append(row[1])
        except ValueError:
            pass

for tid in titles_nand:
    try:
        print('{:016X}: {}'.format(tid, ', '.join(titles_ninupdates[tid])))
    except KeyError:
        print('Missing from ninupdates: {:016x}'.format(tid))
