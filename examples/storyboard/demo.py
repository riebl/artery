#!/usr/bin/env python

# reference variable to Storyboard Omnet++ module: board
import storyboard
import timeline

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
	cond1 = storyboard.TimeCondition(timeline.milliseconds(15000))

	# Create CarSetCondition
	cond2 = storyboard.CarSetCondition(["flow1.0", "flow0.1", "flow0.2"])

	# Create SpeedEffectFactory
	effectFactory0 = storyboard.SpeedEffectFactory(2.44)

	# Create AndConditions
	and0 = storyboard.AndCondition(cond0, cond1)
	and1 = storyboard.AndCondition(and0, cond2)

	# Create OrCondition
	cond3 = storyboard.TimeCondition(timeline.seconds(190))
	or0 = storyboard.OrCondition(cond3, and1)

	# Create Story
	story = storyboard.Story(or0, [effectFactory0])

	# Create Story 2
	cond4 = storyboard.TimeCondition(timeline.seconds(50), timeline.seconds(60))
	effectFactory1 = storyboard.SpeedEffectFactory(2.44)
	story1 = storyboard.Story(cond4, [effectFactory1])

	# Create Story 3, overlapping story0
	cond5 = storyboard.TimeCondition(timeline.seconds(200), timeline.seconds(210))
	cond6 = storyboard.CarSetCondition(["flow0.0", "flow0.1"])
	and2 = storyboard.AndCondition(cond5, cond6)
	effectFactory2 = storyboard.SpeedEffectFactory(0.1)
	story2 = storyboard.Story(and2, [effectFactory2])

	# Create Story 4, SpeedConditionGreater
	cond7 = storyboard.SpeedConditionGreater(4.0)
	cond8 = storyboard.TimeCondition(timeline.seconds(20), timeline.seconds(30))
	and3 = storyboard.AndCondition(cond7, cond8)
	effectFactory3 = storyboard.SpeedEffectFactory(1.0)
	story3 = storyboard.Story(and3, [effectFactory3])

	# Register Stories at the Storyboard
	board.registerStory(story)
	board.registerStory(story1)
	board.registerStory(story2)
	board.registerStory(story3)

	print("Stories loaded!")

