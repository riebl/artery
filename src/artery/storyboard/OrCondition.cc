#include "artery/storyboard/OrCondition.h"

OrCondition::OrCondition(Condition* left, Condition* right) :
    m_left(left), m_right(right)
{
}

ConditionResult OrCondition::ResultVisitor::operator()(bool lhs, bool rhs) const
{
    return (lhs || rhs);
}

ConditionResult OrCondition::ResultVisitor::operator()(std::set<Vehicle*> lhs, bool rhs) const
{
    if (lhs.empty()) {
        return rhs;
    } else {
        return lhs;
    }
}

ConditionResult OrCondition::ResultVisitor::operator()(bool lhs, std::set<Vehicle*> rhs) const
{
    if (rhs.empty()) {
        return lhs;
    } else {
        return rhs;
    }
}

ConditionResult OrCondition::ResultVisitor::operator()(std::set<Vehicle*> lhs, std::set<Vehicle*> rhs) const
{
    std::set<Vehicle*> unite;
    std::set_union(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::inserter(unite , unite.begin()));
    return unite;
}

ConditionResult OrCondition::testCondition(const Vehicle& car)
{
    ConditionResult lhs = m_left->testCondition(car);
    ConditionResult rhs = m_right->testCondition(car);
    return boost::apply_visitor(OrCondition::ResultVisitor(), lhs, rhs);
}

void OrCondition::drawCondition(omnetpp::cCanvas* canvas)
{
    m_left->drawCondition(canvas);
    m_right->drawCondition(canvas);
}
