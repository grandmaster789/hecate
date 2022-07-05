#pragma once

#include <vector>
#include <typeindex>
#include <typeinfo>

namespace hecate::util {
	// container for weak pointers to unique types, with type-based lookup
	// expecting pretty small map sizes - implemented as flat map with linear probing
	class TypeMap {
	public:
		template <typename T>
		void insert(T* ptr); // throws if type is already present

		template <typename T>
		void remove(T* ptr = nullptr); // actual value of the argument is optional but may be useful for type deduction etc

		template <typename T>
		const T* get() const; // will return nullptr if the type is not contained

		template <typename T>
		T* get();

	private:
		std::vector<std::type_index> m_Keys;
		std::vector<void*>           m_Values;
	};
}

#include "typemap.inl"