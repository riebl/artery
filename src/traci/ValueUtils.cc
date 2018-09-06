/*
 * Artery V2X Simulation Framework
 * Copyright 2018 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "traci/ValueUtils.h"

namespace traci
{

using namespace libsumo;

TraCIInt make_value(int i)
{
    TraCIInt v;
    v.value = i;
    return v;
}

TraCIDouble make_value(double scalar)
{
    TraCIDouble d;
    d.value = scalar;
    return d;
}

const TraCIPosition& make_value(const TraCIPosition& pos)
{
    return pos;
}

const TraCIColor& make_value(const TraCIColor& color)
{
    return color;
}

TraCIString make_value(const std::string& str)
{
    TraCIString s;
    s.value = str;
    return s;
}

TraCIStringList make_value(const std::vector<std::string>& strList)
{
    TraCIStringList sl;
    sl.value = strList;
    return sl;
}

TraCIString make_value(std::string&& str)
{
    TraCIString s;
    s.value = std::move(str);
    return s;
}

TraCIStringList make_value(std::vector<std::string>&& strList)
{
    TraCIStringList sl;
    sl.value = std::move(strList);
    return sl;
}

} // namespace libsumo
