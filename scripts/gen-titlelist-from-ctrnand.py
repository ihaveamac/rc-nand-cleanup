#!/usr/bin/env python3

import glob
import sys

if len(sys.argv) < 3:
    sys.exit('{} <title-folder> <output:nand-titles.bin>'.format(sys.argv[0]))

titles_ctrnand = []

tmds = glob.iglob(sys.argv[1] + '/**/*.tmd', recursive=True)
for t in tmds:
    with open(t, 'rb') as f:
        f.seek(0x18C)
        titles_ctrnand.append(f.read(8)[::-1])

with open(sys.argv[2], 'wb') as f:
    f.write(b''.join(titles_ctrnand))
