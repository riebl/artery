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

#include <cassert>
#include <stdexcept>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

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
		typename std::decay<T>::type* get_mutable_ptr() const
		{
			static_assert(std::is_class<T>::value, "T has to be a class type");
			using DT = typename std::decay<T>::type;
			DT* object = nullptr;
			auto found = m_mutable_objects.find(std::type_index(typeid(DT)));
			if (found != m_mutable_objects.end()) {
				object = static_cast<DT*>(found->second);
			}
			return object;
		}

		template<typename T>
		typename std::decay<T>::type& get_mutable() const
		{
			auto obj = get_mutable_ptr<T>();
			if (!obj) throw std::out_of_range("no valid object registered");
			return *obj;
		}

		template<typename T>
		const typename std::decay<T>::type* get_const_ptr() const
		{
			static_assert(std::is_class<T>::value, "T has to be a class type");
			using DT = typename std::decay<T>::type;
			const DT* object = nullptr;
			auto found = m_const_objects.find(std::type_index(typeid(DT)));
			if (found != m_const_objects.end()) {
				object = static_cast<const DT*>(found->second);
			}
			return object;
		}

		template<typename T>
		const typename std::decay<T>::type& get_const() const
		{
			auto obj = get_const_ptr<T>();
			if (!obj) throw std::out_of_range("no valid object registered");
			return *obj;
		}

		template<typename T>
		void register_mutable(T* object)
		{
			assert(object);
			static_assert(std::is_class<T>::value, "T has to be a class type");
			using DT = typename std::decay<T>::type;
			m_mutable_objects[std::type_index(typeid(DT))] = object;
			register_const(object);
		}

		template<typename T>
		void register_const(const T* object)
		{
			assert(object);
			static_assert(std::is_class<T>::value, "T has to be a class type");
			using DT = typename std::decay<T>::type;
			m_const_objects[std::type_index(typeid(DT))] = object;
		}

		// these (deprecated) getters are only provided for compatibility reasons
		const VehicleDataProvider& getVehicleDataProvider() const;
		vanetza::dcc::Scheduler& getDccScheduler();
		const vanetza::dcc::StateMachine& getDccStateMachine() const;

	private:
		std::unordered_map<std::type_index, void*> m_mutable_objects;
		std::unordered_map<std::type_index, const void*> m_const_objects;
};

#endif /* FACILITIES_H_ */
