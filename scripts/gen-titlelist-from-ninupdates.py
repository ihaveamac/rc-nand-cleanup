#!/usr/bin/env python3

import csv
import sys

if len(sys.argv) < 3:
    sys.exit('{} <ninupdates-titles.csv> <output:nand-titles.bin>'.format(sys.argv[0]))

titles_ninupdates = []

with open(sys.argv[1], 'r') as c:
    r = csv.reader(c)
    for row in r:
        try:
            titles_ninupdates.append(int(row[0], 16))
        except ValueError:
            pass

with open(sys.argv[2], 'wb') as f:
    f.write(b''.join(t.to_bytes(8, 'little') for t in titles_ninupdates))
