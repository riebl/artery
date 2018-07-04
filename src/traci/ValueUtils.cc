/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "traci/ValueUtils.h"

namespace libsumo
{

TraCIValue make_value(double scalar)
{
    TraCIValue v;
    v.scalar = scalar;
    return v;
}

TraCIValue make_value(const TraCIPosition& pos)
{
    TraCIValue v;
    v.position = pos;
    return v;
}

TraCIValue make_value(const TraCIColor& color)
{
    TraCIValue v;
    v.color = color;
    return v;
}

TraCIValue make_value(const std::string& str)
{
    TraCIValue v;
    v.string = str;
    return v;
}

TraCIValue make_value(const std::vector<std::string>& strList)
{
    TraCIValue v;
    v.stringList = strList;
    return v;
}

TraCIValue make_value(std::string&& str)
{
    TraCIValue v;
    v.string = std::move(str);
    return v;
}

TraCIValue make_value(std::vector<std::string>&& strList)
{
    TraCIValue v;
    v.stringList = std::move(strList);
    return v;
}

TraCIValue make_value(SUMOTime t)
{
    TraCIValue v;
    v.scalar = t;
    return v;
}

TraCIValue make_value(int i)
{
    TraCIValue v;
    v.scalar = i;
    return v;
}

template<>
typename details::value_cast_trait<double>::return_type
value_cast<double>(const TraCIValue& v)
{
    return v.scalar;
}

template<>
typename details::value_cast_trait<TraCIColor>::return_type
value_cast<TraCIColor>(const TraCIValue& v)
{
    return v.color;
}

template<>
typename details::value_cast_trait<TraCIPosition>::return_type
value_cast<TraCIPosition>(const TraCIValue& v)
{
    return v.position;
}

template<>
typename details::value_cast_trait<std::string>::return_type
value_cast<std::string>(const TraCIValue& v)
{
    return v.string;
}

template<>
typename details::value_cast_trait<std::vector<std::string>>::return_type
value_cast<std::vector<std::string>>(const TraCIValue& v)
{
    return v.stringList;
}

template<>
typename details::value_cast_trait<SUMOTime>::return_type
value_cast<SUMOTime>(const TraCIValue& v)
{
    return v.scalar;
}

template<>
typename details::value_cast_trait<int>::return_type
value_cast<int>(const TraCIValue& v)
{
    return v.scalar;
}

} // namespace libsumo
