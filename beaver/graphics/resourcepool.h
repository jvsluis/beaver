#pragma once

#include <cstdint>
#include <vector>

#include "beaver/core/handle.h"

namespace bvr::gfx {

template <typename T>
class ResourcePool {
public:
    core::Handle<T> insert(T resource) {
        uint32_t index;

        if (free_list_head_ != UINT32_MAX) {
            index = free_list_head_;
            free_list_head_ = slots_[index].next_free;
        } else {
            index = static_cast<uint32_t>(slots_.size());
            slots_.emplace_back();
        }

        Slot& slot = slots_[index];
        slot.resource = std::move(resource);
        slot.active = true;

        return core::Handle<T>{index, slot.generation};
    }

    T* get(core::Handle<T> handle) {
        if (!handle.valid() || handle.index >= slots_.size()) {
            return nullptr;
        }

        Slot& slot = slots_[handle.index];

        // Slot is inactive, or this is a stale handle
        if (!slot.active || slot.generation != handle.generation) {
            return nullptr;
        }

        return &slot.resource;
    }

    void remove(core::Handle<T> handle) {
        if (handle.valid() || handle.index >= slots_.size()) {
            return;
        }

        Slot& slot = slots_[handle.index];

        if (slot.active && slot.generation == handle.generation) {
            slot.active = false;

            // Clear the resource.
            // Since it is wgpu wrapper, this will call underlying release function
            slot.resource = {};

            slot.generation++;
            if (slot.generation == 0) {
                slot.generation = 1;
            }

            slot.next_free = free_list_head_;
            free_list_head_ = handle.index;
        }
    }

    void clear() {
        slots_.clear();
        free_list_head_ = UINT32_MAX;
    }

private:
    struct Slot {
        T resource;
        uint32_t generation = 1;
        uint32_t next_free = UINT32_MAX;
        bool active = false;
    };

    std::vector<Slot> slots_;
    uint32_t free_list_head_ = UINT32_MAX;
};

}  // namespace bvr::gfx
