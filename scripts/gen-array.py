#!/usr/bin/env python3

# THIS DOES NOT GENERATE READY ARRAYS! you must remove titles from it that are shared across all regions.

import struct
import sys

if len(sys.argv) < 3:
    sys.exit('{} <nand-titles.bin> <region>'.format(sys.argv[0]))

with open(sys.argv[1], 'rb') as t:
    title_ids = [tid[0] for tid in struct.iter_unpack('<Q', t.read())]
    title_ids.sort()
    print('const u64 titleIDs_{}[] = {{'.format(sys.argv[2].lower()))
    for tid in title_ids:
        print('\t0x{:016X},'.format(tid))
    print('};')
