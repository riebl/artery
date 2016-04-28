#include "access_category.hpp"
#include <iostream>

namespace vanetza
{

std::ostream& operator<<(std::ostream& os, AccessCategory ac)
{
    switch (ac) {
        case AccessCategory::BK:
            os << "AC_BK";
            break;
        case AccessCategory::BE:
            os << "AC_BE";
            break;
        case AccessCategory::VI:
            os << "AC_VI";
            break;
        case AccessCategory::VO:
            os << "AC_VO";
            break;
        default:
            os << "<unknown AC>";
            break;
    };
    return os;
}

} // namespace vanetza

