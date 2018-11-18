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

#ifndef ARTERY_FACILITIES_H_
#define ARTERY_FACILITIES_H_

#include <cassert>
#include <stdexcept>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <omnetpp/cexception.h>

namespace artery
{

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
		typename std::decay<T>::type* getMutablePtr() const
		{
			return get_mutable_ptr<T>();
		}

		template<typename T>
		typename std::decay<T>::type& get_mutable() const
		{
			auto obj = get_mutable_ptr<T>();
			if (!obj) throw omnetpp::cRuntimeError("no valid object of type '%s' registered", typeid(T).name());
			return *obj;
		}

		template<typename T>
		typename std::decay<T>::type& getMutable() const
		{
			return get_mutable<T>();
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
		const typename std::decay<T>::type* getConstPtr() const
		{
			return get_const_ptr<T>();
		}

		template<typename T>
		const typename std::decay<T>::type& get_const() const
		{
			auto obj = get_const_ptr<T>();
			if (!obj) throw omnetpp::cRuntimeError("no valid object of type '%s' registered", typeid(T).name());
			return *obj;
		}

		template<typename T>
		const typename std::decay<T>::type& getConst() const
		{
			return get_const<T>();
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
		void registerMutable(T* object)
		{
			register_mutable(object);
		}

		template<typename T>
		void register_const(const T* object)
		{
			assert(object);
			static_assert(std::is_class<T>::value, "T has to be a class type");
			using DT = typename std::decay<T>::type;
			m_const_objects[std::type_index(typeid(DT))] = object;
		}

		template<typename T>
		void registerConst(const T* object)
		{
			register_const(object);
		}

	private:
		std::unordered_map<std::type_index, void*> m_mutable_objects;
		std::unordered_map<std::type_index, const void*> m_const_objects;
};

} // namespace artery

#endif /* ARTERY_FACILITIES_H_ */
