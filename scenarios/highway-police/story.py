#!/usr/bin/env python

import storyboard
import timeline

print ("story.py successfully imported...")

def createStories(board):
    # condition triggering after 10 simulated seconds
    timeCondition = storyboard.TimeCondition(timeline.seconds(10))

    # select police car
    carSetCondition = storyboard.CarSetCondition("police0")

    # create signal effect
    signalEffect = storyboard.SignalEffect("siren on")

    # combine conditions
    condition = storyboard.AndCondition(timeCondition, carSetCondition)

    # create story by linking effect and conditions together
    story = storyboard.Story(condition, [signalEffect])

    # activate story
    board.registerStory(story)
