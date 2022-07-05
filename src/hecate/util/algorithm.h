#pragma once

#include <array>
#include <array>
#include <vector>
#include <memory>
#include <optional>
#include <tuple>
#include <iterator>

namespace hecate::util {
	// 
	// vector<unique_ptr<T>>   ~~~>   vector<T*>
	// 
	// (not compatible with *all* STL containers, just all sequence containers that is: 
	//  array, vector, deque, list and forward_list. Slightly optimized for vector)
	// 
	template <
		template <typename, typename> class t_Container,
		typename                            t_Element,
		typename                            t_AllocatorA,
		typename                            t_AllocatorB = std::allocator<t_Element*>
	>
	t_Container<t_Element*, t_AllocatorB> create_non_owning_copy(
		const t_Container<std::unique_ptr<t_Element>, t_AllocatorA>& x
	);

	template <typename T>
	std::vector<T> raw_to_vector(
		T*     raw_array,
		size_t num_elements
	);

	// reduced typing for a lot of vulkan arguments...
	template <typename T>
	uint32_t vec_count(const std::vector<T>& v);

	template <typename t_Container, typename t_Element>
	bool contains(
		const t_Container& c,
		const t_Element& x
	);

	template <typename t_Container, typename tPredicate>
	bool contains_if(
		const t_Container& c,
		tPredicate  pred
	);

	template <typename t_Container>
	bool contains_all_of(
		const t_Container& a,
		const t_Container& b
	);

	template <typename t_Container, typename t_Element>
	typename t_Container::const_iterator find(
		const t_Container& c,
		const t_Element& value
	);

	template <typename t_Container, typename tPredicate>
	typename t_Container::const_iterator find_if(
		const t_Container& c,
		tPredicate  pred
	);

	template <typename t_IteratorA, typename t_IteratorB>
	constexpr t_IteratorA find_any_of(
		t_IteratorA haystack_first,
		t_IteratorA haystack_last,
		t_IteratorB needles_first,
		t_IteratorB needles_last
	) noexcept; // returns haystack_last if none was found

	template <typename t_Container, typename t_Element>
	bool erase(
		t_Container& c,
		const t_Element& value
	); // returns true if the value was erased (false if it wasn't found)

	template <typename t_Container, typename t_Predicate>
	bool erase_if(
		t_Container& vec,
		t_Predicate  pred
	); // returns true if something was erased

	template <typename t_Container>
	void sort(t_Container& c);

	template <typename t_Container>
	void uniquify(t_Container& c); // eliminates duplicate values from a container (also sorts it)

	template <typename T, typename U>
	bool assign_if_changed(T& old_value, U&& new_value) noexcept; // returns true if the value was changed

	template <
		typename t_ContainerA,
		typename t_ContainerB,
		typename t_Functor
	>
	t_ContainerA transform(
		const t_ContainerB& source,
		t_Functor fn
	);

	template <typename C, typename V>
	constexpr void fill(C& container, const V& value);

	template <
		typename T,
		size_t   N,
		typename tFunctor // fn(size_t) -> T
	>
	constexpr std::array<T, N> generate_array(tFunctor fn);

	template <typename T, size_t N>
	constexpr uint32_t count_of(T(&)[N]);

	// given a set of options, select one if it is available with any give number of fallbacks
	// (yields nullopt if none of the preferences was in the available set)
	template <typename C, typename...t_Values>
	std::optional<typename C::value_type> prefer(
		const C&           available_options,
		const t_Values&... preferred
	);
}

#include "algorithm.inl"