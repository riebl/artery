#!/usr/bin/env python

# reference variable to Storyboard Omnet++ module: board
import storyboard

print ("demo.py successfully imported...")

def createStories():
	# Create coordinates needed for the PolygonCondition
	coord0 = storyboard.Coord(0.0, 0.0, 0.0)
	coord1 = storyboard.Coord(3000.0, 0.0, 0.0)
	coord2 = storyboard.Coord(3000.0, 1600.0, 0.0)
	coord3 = storyboard.Coord(0.0, 1600.0, 0.0)

	# Create PolygonCondition
	cond0 = storyboard.PolygonCondition([coord0, coord1, coord2, coord3])

	# Create TimeCondition
	cond1 = storyboard.TimeCondition(storyboard.SimTime(10))

	# Create SpeedEffectFactory
	effectFactory0 = storyboard.SpeedEffectFactory(2.44)

	# Create Story
	story = storyboard.Story([cond0, cond1],[effectFactory0])

	# Register Story at the Storyboard
	board.registerStory(story)

	print("Stories loaded!")

