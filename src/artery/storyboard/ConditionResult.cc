#include "artery/storyboard/ConditionResult.h"
#include <boost/variant/static_visitor.hpp>

namespace artery
{

bool is_true(const ConditionResult& result)
{
    struct visitor : boost::static_visitor<bool>
    {
        bool operator()(bool b) const {
            return b;
        }

        bool operator()(const std::set<const Vehicle*>& s) const {
            return !s.empty();
        }
    };

    return boost::apply_visitor(visitor(), result);
}

} // namespace artery
