#pragma once

namespace hecate::util {
    template <typename C, typename V>
    requires (std::convertible_to<V, typename C::value_type>)
    auto find(
        const C& container,
        const V& value
    );

    template <typename C, typename P>
    auto find_if(
        const C& container,
        P        predicate
    );

    template <typename C, typename V>
    requires (std::convertible_to<V, typename C::value_type>)
    bool contains(
        const C& container,
        const V& value
    );

    template <typename C, typename P>
    bool contains_if(
        const C& container,
        P        predicate
    );

    // from CppCon 2019: V. Romeo "Higher-order functions in Modern C++: existing techniques and function_ref"
    // (given a set of values, if a value passes a predicate call a callback and remove it from the set)
    template <typename C, typename P, typename Fn>
    void consume_if(
        C&&  range,
        P&&  predicate,
        Fn&& callback
    );
}

#include "algorithm.inl"
