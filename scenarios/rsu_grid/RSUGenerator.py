#!/usr/bin/env python
import argparse
import math
import numpy
import os
import sys
from lxml import etree

if 'SUMO_HOME' in os.environ:
    tools = os.path.join(os.environ['SUMO_HOME'], 'tools')
    sys.path.append(tools)
else:
    sys.exit("please declare environment variable 'SUMO_HOME'")

import sumolib  # noqa


parser = argparse.ArgumentParser(
    description='RSU XML generation script for junctions.',
    epilog="""This script reads junctions coordinates and their edges from
        SUMO network XML file. Generates a RSU in the center of every junction
        and adds an antenna for each outgoing edge.""")

parser.add_argument(
    '-n', help='SUMO network file to parse',
    type=str, metavar='<path>/<scenario>.net.xml', required=True)

parser.add_argument(
    '-o', help='File to write RSU definitions to',
    type=str, metavar='<path>/RSU.xml', required=True)


args = parser.parse_args()
net = sumolib.net.readNet(args.n)
netBBox = net.getBBoxXY()
junctionCount = 0

root = etree.Element("RSUs")
root.append(etree.Comment(
    "This file was generated with '{}'".format(" ".join(sys.argv))))

for junction in net.getNodes():
    junctionCount += 1

    nodePos = junction.getCoord()
    oppPosX = nodePos[0] - netBBox[0][0]
    oppPosY = netBBox[1][1] - nodePos[1]

    doc = etree.SubElement(
        root, "rsu",
        id=junction.getID(),
        positionX=str(oppPosX), positionY=str(oppPosY))

    for edge in junction.getOutgoing():
        outgoingNodePos = edge.getToNode().getCoord()
        directionVector = numpy.subtract(outgoingNodePos, nodePos)
        length = numpy.linalg.norm(directionVector)
        normalizedDirectionVector = [pos / length for pos in directionVector]
        normalizedDirectionVector = [normalizedDirectionVector[0],
                                     normalizedDirectionVector[1] * -1]
        direction = numpy.arctan2(normalizedDirectionVector[1],
                                  normalizedDirectionVector[0])

        if(direction == math.pi * -1):
            direction += 2 * math.pi

        if(direction == -0):
            direction = 0

        antenna = etree.SubElement(doc, "antenna", direction=str(direction))

    tree = etree.ElementTree(root)
    tree.write(args.o, pretty_print=True)
