#!/usr/bin/env/ python

import sys

def strip(inp, outp):
    with open(inp) as log:
        lines = log.read().splitlines()

    for line in lines:
        s = line.replace("00:00:0", "")
        if "date" not in s:
            outp.write(s + "\n")

logfile1 = "realTime.log"
logfile2 = "eventTime.log"

outputSuffix1 = "RealTime.txt"
outputSuffix2 = "EventTime.txt"

outputPrefix = str(sys.argv[1])

outfile1 = open(outputPrefix + outputSuffix1, 'w')
outfile2 = open(outputPrefix + outputSuffix2, 'w')


strip(logfile1, outfile1)
strip(logfile2, outfile2)


print(outputPrefix)




