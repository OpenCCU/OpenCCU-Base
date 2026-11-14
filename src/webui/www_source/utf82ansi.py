#!/usr/bin/python

import sys
import codecs

fileName  = sys.argv[1]

inputFile = codecs.open(fileName, "rb", "utf-8", errors="ignore")
input     = inputFile.read()
inputFile.close()

outputFile = codecs.open(fileName, "wb", "iso-8859-1", errors="ignore")
outputFile.write(input)
outputFile.close()
