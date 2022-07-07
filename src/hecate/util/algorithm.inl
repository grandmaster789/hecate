#pragma once

#include "algorithm.h"
#include <algorithm>

namespace hecate::util {
    template <typename C, typename V>
    requires (std::convertible_to<V, typename C::value_type>)
    auto find(
            const C& container,
            const V& value
    ) {
        return std::find(
            std::begin(container),
            std::end(container),
            value
        );
    }

    template <typename C, typename P>
    auto find_if(
            const C& container,
            P        predicate
    ) {
        return std::find_if(
            std::begin(container),
            std::end(container),
            predicate
        );
    }

    template <typename C, typename V>
    requires (std::convertible_to<V, typename C::value_type>)
    bool contains(
        const C& container,
        const V& value
    ) {
        return std::find(
            std::begin(container),
            std::end(container),
            value
        ) != std::end(container);
    }

    template <typename C, typename P>
    bool contains_if(
        const C& container,
        P        predicate
    ) {
        return std::find_if(
                std::begin(container),
                std::end(container),
                predicate
        ) != std::end(container);
    }

    template <typename C, typename P, typename Fn>
    void consume_if(
        C&&  range,
        P&&  predicate,
        Fn&& callback
    ) {
        for (auto it = std::begin(range); it != std::end(range);) {
            if (predicate(*it)) {
                callback(*it);
                it = range.erase(it);
            }
            else
                ++it;
        }
    }
}