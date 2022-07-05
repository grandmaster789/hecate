#include "algorithm.h"
#include <algorithm>

namespace hecate::util {
	template <
		template <typename, typename> class C,
		typename T,
		typename A,
		typename B
	>
	C<T*, B> create_non_owning_copy(const C<std::unique_ptr<T>, A>& container) {
		C<T*, B> result;

		// TODO possibly we could try and figure out if the container has a .reserve() method instead
		if constexpr (std::is_same_v<std::vector<T*>, decltype(result)>)
		{
			result.reserve(container.size());
		}

		for (auto& entry : container)
			result.push_back(entry.get());

		return result;
	}

	template <typename T>
	std::vector<T> raw_to_vector(T* raw_array, size_t num_elements) {
		// vector has 10 constructors, one of which does what we need here...
		// https://en.cppreference.com/w/cpp/container/vector/vector (#5)
		return std::vector<T>(
			raw_array,
			raw_array + num_elements
		);
	}

	template <typename T>
	uint32_t vec_count(const std::vector<T>& v) {
		return static_cast<uint32_t>(v.size());
	}

	template <typename C, typename E>
	bool contains(const C& c, const E& x) {
		return std::find(
			std::begin(c),
			std::end(c),
			x
		) != std::end(c);
	}

	template <typename C, typename P>
	bool contains_if(
		const C& c,
		P  pred
	) {
		return std::find_if(
			std::begin(c),
			std::end(c),
			pred
		) != std::end(c);
	}

	template <typename C>
	bool contains_all_of(
		const C& a,
		const C& b
	) {
		for (const auto& value : b)
			if (!contains(a, value))
				return false;

		return true;
	}

	template <typename C, typename E>
	typename C::const_iterator find(
		const C& c,
		const E& value
	) {
		return std::find(
			std::begin(c),
			std::end(c),
			value
		);
	}

	template <typename C, typename Pred>
	typename C::const_iterator find_if(
		const C& c,
		Pred     pred
	) {
		return std::find_if(
			std::begin(c),
			std::end(c),
			pred
		);
	}

	template <typename t_IteratorA, typename t_IteratorB>
	constexpr t_IteratorA find_any_of(
		t_IteratorA haystack_first,
		t_IteratorA haystack_last,
		t_IteratorB needles_first,
		t_IteratorB needles_last
	) noexcept {
		return std::find_if(
			haystack_first,
			haystack_last,
			[needles_first, needles_last](const auto& hay) {
				return std::any_of(
					needles_first,
					needles_last,
					[&hay](const auto& needle) {
						return (needle == hay);
					}
				);
			}
		);
	}

	template <typename t_Container, typename t_Element>
	bool erase(
		t_Container& c,
		const t_Element& value
	) {
		auto it = std::remove(
			std::begin(c),
			std::end(c),
			value
		);

		if (it == std::end(c))
			return false;
		else
			c.erase(it, std::end(c));

		return true;
	}

	template <typename t_Container, typename t_Predicate>
	bool erase_if(
		t_Container& c,
		t_Predicate  pred
	) {
		auto it = std::remove_if(
			std::begin(c),
			std::end(c),
			std::forward<t_Predicate>(pred)
		);

		if (it == std::end(c))
			return false;
		else
			c.erase(it, std::end(c));

		return true;
	}

	template <typename C>
	void sort(C& c) {
		std::sort(
			std::begin(c),
			std::end(c)
		);
	}

	template <typename C>
	void uniquify(C& c) {
		sort(c);

		auto it = std::unique(
			std::begin(c),
			std::end(c)
		);

		c.erase(
			it,
			std::end(c)
		);
	}

	template <typename T, typename U>
	bool assign_if_changed(T& old_value, U&& new_value) noexcept {
		if (old_value == new_value)
			return false;

		old_value = std::forward<U>(new_value);
		return true;
	}

	template <typename T, typename U, typename Fn>
	T transform(const U& source, Fn fn) {
		T result;
		result.reserve(source.size());

		std::transform(
			std::begin(source),
			std::end(source),
			std::back_inserter(result),
			fn
		);

		return result;
	}

	template <typename C, typename V>
	constexpr void fill(C& container, const V& value) {
		std::fill(
			std::begin(container),
			std::end(container),
			value
		);
	}

	template <typename T, size_t N, typename Fn>
	constexpr std::array<T, N> generate_array(Fn fn) {
		std::array<T, N> result;

		for (size_t i = 0; i < N; ++i)
			result[i] = fn(i);

		return result;
	}

	template <typename T, size_t N>
	constexpr uint32_t count_of(T(&)[N]) {
		static_assert(N < std::numeric_limits<uint32_t>::max());
		return static_cast<uint32_t>(N);
	}

	namespace detail {
		template <typename C, typename T, typename... Vs>
		std::optional<typename C::value_type> prefer_impl(
			const C& haystack,
			const T& current_needle,
			const Vs&... remaining_needles
		) {
			static_assert(std::is_same_v<typename C::value_type, T>);

			if (contains(haystack, current_needle))
				return current_needle;

			if constexpr (sizeof...(Vs) > 0) {
				return prefer_impl(
					haystack,
					remaining_needles...
				);
			}
			else // if this 'else' is missing, MSVC will yield 'unreachable code' warnings?
				return std::nullopt;
		}
	}

	template <typename C, typename...tValues>
	std::optional<typename C::value_type> prefer(
		const C& available_options,
		const tValues&... preferred
	) {
		if constexpr (sizeof...(tValues) == 0) {
			return std::nullopt;
		}

		return detail::prefer_impl(
			available_options,
			preferred...
		);
	}
}