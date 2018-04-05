import storyboard as sb

def createStories(board):
    region = sb.PolygonCondition([
        sb.Coord(300, 300), sb.Coord(700, 300),
        sb.Coord(700, 700), sb.Coord(300, 700)])
    fast = sb.SpeedConditionGreater(16.67)
    blackice = sb.AndCondition(region, fast)
    story = sb.Story(blackice, [sb.SignalEffect("traction loss")])
    board.registerStory(story)
