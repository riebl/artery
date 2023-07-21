#ifndef ARTERY_CONDITIONRESULT_H_2UBLV1EE
#define ARTERY_CONDITIONRESULT_H_2UBLV1EE

#include "artery/storyboard/Macros.h"
#include <boost/variant.hpp>
#include <set>

namespace artery
{

// forward declaration
class Vehicle;

using ConditionResult = boost::variant<bool, std::set<const Vehicle*>>;
bool STORYBOARD_API is_true(const ConditionResult&);

} // namespace artery

#endif /* ARTERY_CONDITIONRESULT_H_2UBLV1EE */

