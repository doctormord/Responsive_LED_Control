#!/usr/local/bin/python

# Griswold is a fork of the LEDLAMP project at 
#         https://github.com/russp81/LEDLAMP_FASTLEDs

# The LEDLAMP project is a fork of the McLighting Project at
#         https://github.com/toblum/McLighting

# PaletteKnife was released under the MIT License (MIT), and hence this is as well.

# Copyright (c) 2016 @jake-b

# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# This is a rudamentary clone of PaletteKnife, designed to output the palette to a
# file in binary format.

import sys
import re
import math
import os

def adjustGamma(orig, gamma):
    o = orig / 255.0;
    adj = math.pow( o, gamma);
    res = math.floor( adj * 255.0);
    if ((orig != 0) and (res == 0)):
        res = 1;
    return int(res);


infile = sys.argv[1]
outfile = os.path.splitext(infile)[0] + ".bin"

print "Processing file: " + infile

with open(infile) as f:
    content = f.read()
    output_bytes = []

    regex = re.compile('.*\(\s*([0-9]+), *([0-9]+), *([0-9]+)\)\s+([0-9.]+)')

    # RGBA Warning
    if content.find("rgba(") != -1:
        print("WARNING: TRANSPARENCY not supported.");

    count = 0
    for line in content.split('\n'):
        match = regex.match(line)

        if match:
            #print len(match)
            r = int(match.group(1))
            g = int(match.group(2))
            b = int(match.group(3))
            pct = float(match.group(4))
            ndx = int(math.floor( (pct * 255.0) / 100.0 ))
            
            output_bytes.append(ndx)
            output_bytes.append(adjustGamma(r, 2.6))
            output_bytes.append(adjustGamma(g, 2.2))
            output_bytes.append(adjustGamma(b, 2.5))

    f.close()

    newFileByteArray = bytearray(output_bytes)
    with open(outfile,'wb') as newFile:
        newFile.write(newFileByteArray)
        newFile.close()




