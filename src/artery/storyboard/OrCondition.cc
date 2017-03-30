#include "artery/storyboard/OrCondition.h"

class OrVisitor : public boost::static_visitor<ConditionResult>
{
public:
    ConditionResult operator()(bool lhs, bool rhs) const {
        return lhs || rhs;
    }

    ConditionResult operator()(const std::set<const Vehicle*>& lhs, bool rhs) const {
        if (lhs.empty()) {
            return rhs;
        } else {
            return lhs;
        }
    }

    ConditionResult operator()(bool lhs, const std::set<const Vehicle*>& rhs) const {
        if (rhs.empty()) {
            return lhs;
        } else {
            return rhs;
        }
    }

    ConditionResult operator()(const std::set<const Vehicle*>& lhs, const std::set<const Vehicle*>& rhs) const {
        std::set<const Vehicle*> unite;
        std::set_union(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
                std::inserter(unite , unite.begin()));
        return unite;
    }
};


OrCondition::OrCondition(Condition* left, Condition* right) :
    m_left(left), m_right(right)
{
}

ConditionResult OrCondition::testCondition(const Vehicle& car)
{
    ConditionResult lhs = m_left->testCondition(car);
    if (!is_true(lhs)) {
        ConditionResult rhs = m_right->testCondition(car);
        return boost::apply_visitor(OrVisitor(), lhs, rhs);
    } else {
        return lhs;
    }
}

void OrCondition::drawCondition(omnetpp::cCanvas* canvas)
{
    m_left->drawCondition(canvas);
    m_right->drawCondition(canvas);
}
