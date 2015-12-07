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

#ifndef FACILITIES_H_
#define FACILITIES_H_

#include <stdexcept>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>

// forward declarations (for deprecated getters)
class VehicleDataProvider;
namespace Veins { class TraCIMobility; }
namespace vanetza {
namespace dcc {
	class Scheduler;
	class StateMachine;
} // ns dcc
} // ns vanetza

/**
 * Context class for each ITS-G5 service provided by middleware
 */
class Facilities
{
	public:
		template<typename T>
		T* get()
		{
			auto index = std::type_index(typeid(T));
			if (m_const_objects.find(index) != m_const_objects.end()) {
				throw std::logic_error("Illegal mutable access to const object");
			}
			return static_cast<T*>(m_objects.at(index));
		}

		template<typename T>
		const T* get() const
		{
			auto index = std::type_index(typeid(T));
			return static_cast<const T*>(m_objects.at(index));
		}

		template<typename T>
		void register_mutable(T* object)
		{
			m_objects[std::type_index(typeid(T))] = object;
		}

		template<typename T>
		void register_const(const T* object)
		{
			auto index = std::type_index(typeid(T));
			m_objects[index] = const_cast<T*>(object);
			m_const_objects.insert(index);
		}

		// these (deprecated) getters are only provided for compatibility reasons
		const VehicleDataProvider& getVehicleDataProvider() const;
		Veins::TraCIMobility& getMobility();
		vanetza::dcc::Scheduler& getDccScheduler();
		const vanetza::dcc::StateMachine& getDccStateMachine();

	private:
		std::unordered_map<std::type_index, void*> m_objects;
		std::unordered_set<std::type_index> m_const_objects;
};

#endif /* FACILITIES_H_ */
