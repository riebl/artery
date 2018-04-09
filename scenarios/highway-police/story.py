#!/usr/bin/env python

# reference variable to Storyboard Omnet++ module: board
import storyboard
import timeline

print ("story.py successfully imported...")

def createStories(board):
    #create a timeCondition
    timeCondition = storyboard.TimeCondition(timeline.seconds(10))

    #choose cars where the story should be active (police car)
    carSetCondition = storyboard.CarSetCondition("police0")

    #create signal Effect
    signalEffect = storyboard.SignalEffect("policeTrigger")

    #combine conditions
    condition = storyboard.AndCondition(timeCondition, carSetCondition)

    #create signal Effect
    signalEffect = storyboard.SignalEffect("policeTrigger")

    #create story by linking effect and conditions together
    story = storyboard.Story(condition, [signalEffect])

    #activate story
    board.registerStory(story)
