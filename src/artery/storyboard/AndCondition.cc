#include "artery/storyboard/AndCondition.h"

namespace artery
{

class AndVisitor : public boost::static_visitor<ConditionResult>
{
public:
    ConditionResult operator()(bool lhs, bool rhs) const {
        return lhs && rhs;
    }

    ConditionResult operator()(const std::set<const Vehicle*>& lhs, bool rhs) const {
        if(!rhs) {
            return false;
        } else {
            return lhs;
        }
    }

    ConditionResult operator()(bool lhs, const std::set<const Vehicle*>& rhs) const {
        if(!lhs) {
            return false;
        } else {
            return rhs;
        }
    }

    ConditionResult operator()(std::set<const Vehicle*> lhs, std::set<const Vehicle*> rhs) const {
         std::set<const Vehicle*> intersect;
         std::set_intersection(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
                 std::inserter(intersect, intersect.begin()));
         return intersect;
    }
};


AndCondition::AndCondition(ConditionPtr left, ConditionPtr right) :
    m_left(left), m_right(right)
{
}

ConditionResult AndCondition::testCondition(const Vehicle& car)
{
    ConditionResult lhs = m_left->testCondition(car);
    if (is_true(lhs)) {
        ConditionResult rhs = m_right->testCondition(car);
        return boost::apply_visitor(AndVisitor(), lhs, rhs);
    } else {
        return lhs;
    }
}

void AndCondition::drawCondition(omnetpp::cCanvas* canvas)
{
    m_left->drawCondition(canvas);
    m_right->drawCondition(canvas);
}

} // namespace artery
