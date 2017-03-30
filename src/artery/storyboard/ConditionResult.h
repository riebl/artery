#ifndef CONDITIONRESULT_H_2UBLV1EE
#define CONDITIONRESULT_H_2UBLV1EE

#include <boost/variant.hpp>
#include <set>

// forward declaration
class Vehicle;

using ConditionResult = boost::variant<bool, std::set<const Vehicle*>>;
bool is_true(const ConditionResult&);

#endif /* CONDITIONRESULT_H_2UBLV1EE */

