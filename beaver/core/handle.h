#pragma once

#include <cstdint>

namespace bvr::core {

template <typename T>
struct Handle {
    // 0 is used to represent an invalid handle
    uint32_t index = 0;
    uint32_t generation = 0;

    bool valid() const {
        return generation > 0;
    }

    bool operator==(const Handle& other) const {
        return index == other.index && generation == other.generation;
    }

    bool operator!=(const Handle& other) const {
        return !(*this == other);
    }
};

}  // namespace bvr::core
