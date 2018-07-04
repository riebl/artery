/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef VALUEUTILS_H_UXDE2KJB
#define VALUEUTILS_H_UXDE2KJB

#include "traci/sumo/libsumo/TraCIDefs.h"

namespace libsumo
{

TraCIValue make_value(double scalar);
TraCIValue make_value(const TraCIPosition& pos);
TraCIValue make_value(const TraCIColor& color);
TraCIValue make_value(const std::string& str);
TraCIValue make_value(const std::vector<std::string>& strList);
TraCIValue make_value(std::string&& str);
TraCIValue make_value(std::vector<std::string>&& strList);
TraCIValue make_value(SUMOTime);
TraCIValue make_value(int);

namespace details
{

template<typename T>
struct value_cast_trait
{
    using return_type = const T&;
};

template<>
struct value_cast_trait<SUMOTime>
{
    using return_type = const SUMOTime;
};

template<>
struct value_cast_trait<int>
{
    using return_type = int;
};

} // namespace details

template<typename T>
typename details::value_cast_trait<T>::return_type value_cast(const TraCIValue&);

template<>
typename details::value_cast_trait<double>::return_type
value_cast<double>(const TraCIValue& v);

template<>
typename details::value_cast_trait<TraCIColor>::return_type
value_cast<TraCIColor>(const TraCIValue& v);

template<>
typename details::value_cast_trait<TraCIPosition>::return_type
value_cast<TraCIPosition>(const TraCIValue& v);

template<>
typename details::value_cast_trait<std::string>::return_type
value_cast<std::string>(const TraCIValue& v);

template<>
typename details::value_cast_trait<std::vector<std::string>>::return_type
value_cast<std::vector<std::string>>(const TraCIValue& v);

template<>
typename details::value_cast_trait<SUMOTime>::return_type
value_cast<SUMOTime>(const TraCIValue& v);

template<>
typename details::value_cast_trait<int>::return_type
value_cast<int>(const TraCIValue& v);

} // namespace libsumo

#endif /* VALUEUTILS_H_UXDE2KJB */

