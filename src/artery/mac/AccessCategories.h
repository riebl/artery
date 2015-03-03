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

#ifndef EDCA_ACCESS_CATEGORIES_H_0RRX3OUI
#define EDCA_ACCESS_CATEGORIES_H_0RRX3OUI

#include "veins/modules/utility/Consts80211p.h"

namespace edca {

enum AccessCategory {
	AC_BK = 0,
	AC_BE = 1,
	AC_VI = 2,
	AC_VO = 3
};

namespace detail {
template<int ArbitraryIFSN, int CWMIN, int CWMAX>
struct AccessCategoryTraits {
		static const int AIFSN = ArbitraryIFSN;
		static const int CW_min = CWMIN;
		static const int CW_max = CWMAX;
};
} // namespace detail

template<int AC>
struct AccessCategoryTraits;

template<>
struct AccessCategoryTraits<AC_BK> :
public detail::AccessCategoryTraits<9, CWMIN_11P, CWMAX_11P> {};

template<>
struct AccessCategoryTraits<AC_BE> :
public detail::AccessCategoryTraits<6, CWMIN_11P, CWMAX_11P> {};

template<>
struct AccessCategoryTraits<AC_VI> :
public detail::AccessCategoryTraits<3, (CWMIN_11P+1)/2-1, CWMIN_11P> {};

template<>
struct AccessCategoryTraits<AC_VO> :
public detail::AccessCategoryTraits<2, (CWMIN_11P+1)/4-1, (CWMIN_11P+1)/2-1> {};

} // namespace edca

#endif /* EDCA_ACCESS_CATEGORIES_H_0RRX3OUI */

