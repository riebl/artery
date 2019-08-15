#!/usr/bin/env python
import os
import sys

if 'SUMO_HOME' in os.environ:
	tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
	sys.path.append(tools)
else:   
	sys.exit("please declare environment variable 'SUMO_HOME'")


import argparse
import sumolib
import numpy
import math
from lxml import etree


parser = argparse.ArgumentParser(
	description='RSU XML generation script for junctions.',
	epilog="""This script reads junctions coordinates and their edges from SUMO network XML file. 
				Generates a RSU in the center of every junction and adds an antenna for each outgoing edge.""")

parser.add_argument('-n',
    help='SUMO network file to parse', 
	type=str, metavar='<path>/<scenario>.net.xml', required=True)

parser.add_argument('-o',
    help='File to write RSU definitions to', 
	type=str, metavar='<path>/RSU.xml', required=True)

	
args = parser.parse_args()

net = sumolib.net.readNet(args.n)

junctionCount = 0

root = etree.Element("RSUs")

root.append(etree.Comment("This file was generated with \'" + " ".join(sys.argv) + "\'"))

for junction in net.getNodes():
	junctionCount += 1

	nodePos = junction.getCoord()

	doc = etree.SubElement(root, "rsu", junctionID=junction.getID() , positionX=str(nodePos[0]), positionY=str(nodePos[1]))

	for edge in junction.getOutgoing():
		outgoingNodePos = edge.getToNode().getCoord()
		directionVector = tuple(numpy.subtract(outgoingNodePos, nodePos))
		length = numpy.linalg.norm(directionVector)
		normalizedDirectionVector = tuple(pos/length for pos in directionVector)
		normalizedDirectionVector = tuple((normalizedDirectionVector[0], normalizedDirectionVector[1] * -1))
		direction = numpy.arctan2(normalizedDirectionVector[1], normalizedDirectionVector[0])

		if(direction == math.pi * -1):
			direction += 2 * math.pi

		if(direction == -0):
			direction = 0

		antenna = etree.SubElement(doc, "antenna", direction=str(direction))

tree = etree.ElementTree(root)

tree.write(args.o, pretty_print=True)