/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef VALUEUTILS_H_UXDE2KJB
#define VALUEUTILS_H_UXDE2KJB

#include "traci/sumo/libsumo/TraCIDefs.h"
#include "traci/VariableTraits.h"
#include <cassert>
#include <utility>

namespace traci
{

libsumo::TraCIInt make_value(int);
libsumo::TraCIDouble make_value(double scalar);
const libsumo::TraCIColor& make_value(const libsumo::TraCIColor& color);
const libsumo::TraCIPosition& make_value(const libsumo::TraCIPosition& pos);
libsumo::TraCIString make_value(const std::string& str);
libsumo::TraCIString make_value(std::string&& str);
libsumo::TraCIStringList make_value(const std::vector<std::string>& strList);
libsumo::TraCIStringList make_value(std::vector<std::string>&& strList);

inline int get_value(const libsumo::TraCIInt& i) { return i.value; }
inline double get_value(const libsumo::TraCIDouble& d) { return d.value; }
inline const libsumo::TraCIColor& get_value(const libsumo::TraCIColor& c) { return c; }
inline const libsumo::TraCIPosition& get_value(const libsumo::TraCIPosition& p) { return p; }
inline const std::string& get_value(const libsumo::TraCIString& s) { return s.value; }
inline const std::vector<std::string>& get_value(const libsumo::TraCIStringList& sl) { return sl.value; }

template<typename T>
struct get_value_trait
{
    using result_type = typename TraCIResultTrait<T>::result_type;
    using return_type = decltype(get_value(std::declval<const result_type&>()));
};

template<typename T>
typename get_value_trait<T>::return_type
get_value(const libsumo::TraCIResult& tr)
{
    using result_type = typename get_value_trait<T>::result_type;
    auto ptr = dynamic_cast<const result_type*>(&tr);
    assert(ptr != nullptr);
    return get_value(*ptr);
}

template<typename T>
typename get_value_trait<T>::return_type
get_value(const std::shared_ptr<const libsumo::TraCIResult>& ptr)
{
    assert(ptr);
    return get_value<T>(*ptr);
}

} // namespace traci

#endif /* VALUEUTILS_H_UXDE2KJB */

