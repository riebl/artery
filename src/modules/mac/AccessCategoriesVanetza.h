//
// Copyright (C) 2014 Raphael Riebl <raphael.riebl@thi.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef EDCA_ACCESS_CATEGORIES_VANETZA_H_KGCFGNQA
#define EDCA_ACCESS_CATEGORIES_VANETZA_H_KGCFGNQA

#include "AccessCategories.h"
#include <vanetza/net/access_category.hpp>
#include <simutil.h>

namespace edca
{

inline AccessCategory map(vanetza::AccessCategory ac)
{
	typedef vanetza::AccessCategory AC;
	AccessCategory result = AC_BK;

	switch (ac) {
		case AC::VI:
			result = AC_VI;
			break;
		case AC::VO:
			result = AC_VO;
			break;
		case AC::BE:
			result = AC_BE;
			break;
		case AC::BK:
			result = AC_BK;
			break;
		default:
			opp_error("AC mapping failure");
			break;
	}

	return result;
}

inline vanetza::AccessCategory map(AccessCategory ac)
{
	typedef vanetza::AccessCategory AC;
	AC result = AC::BK;

	switch (ac) {
		case AC_VI:
			result = AC::VI;
			break;
		case AC_VO:
			result = AC::VO;
			break;
		case AC_BE:
			result = AC::BE;
			break;
		case AC_BK:
			result = AC::BK;
			break;
		default:
			opp_error("AC mapping failure");
			break;
	}

	return result;
}

} // namespace edca

#endif /* EDCA_ACCESS_CATEGORIES_VANETZA_H_KGCFGNQA */
