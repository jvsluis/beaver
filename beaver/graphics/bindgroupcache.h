#pragma once

#include <webgpu/webgpu_cpp.h>

#include <unordered_map>
#include <vector>

#include "beaver/app/applicationcontext.h"
#include "beaver/core/log.h"

namespace bvr::gfx {

template <size_t N>
struct BindGroupCacheKey {
    core::Handle<Texture> textures[N];

    bool operator==(const BindGroupCacheKey& other) const {
        for (int i = 0; i < N; ++i) {
            // This relies on the operator== that is defined in Handle<T>
            // It strictly compares BOTH index and generation.
            if (textures[i] != other.textures[i]) return false;
        }
        return true;
    }
};

template <size_t N>
struct BindGroupCacheKeyHash {
    std::size_t operator()(const BindGroupCacheKey<N>& key) const {
        size_t seed = 0;

        // Standard hash combine function
        auto hash_combine = [&seed](size_t hashValue) {
            seed ^= hashValue + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        };

        for (int i = 0; i < N; ++i) {
            // Pack the 32-bit generation and 32-bit index into one 64-bit number
            uint64_t handleData =
                (static_cast<uint64_t>(key.textures[i].generation) << 32) |
                static_cast<uint64_t>(key.textures[i].index);

            hash_combine(std::hash<uint64_t>{}(handleData));
        }

        return seed;
    }
};

struct CachedBindGroup {
    wgpu::BindGroup bindgroup;
    uint64_t last_used_frame = 0;
};

template <size_t N>
class BindGroupCache {
public:
    BindGroupCache() = default;
    ~BindGroupCache() = default;

    void create(bvr::app::ApplicationContext& context, wgpu::BindGroupLayout layout) {
        context_ = &context;

        bindgroup_layout_ = layout;
    }

    void start_frame() {
        current_frame_++;
    }

    void garbage_collect() {
        const int max_unused_frames = 10;
        if (current_frame_ % 60 == 0) {
            for (auto it = map_.begin(); it != map_.end();) {
                uint64_t frames_since_last_use = current_frame_ - it->second.last_used_frame;

                if (frames_since_last_use > max_unused_frames) {
                    // The bind group is stale
                    // erasing from map will decrement ref count and release it
                    it = map_.erase(it);
                    CORE_INFO("Garbage collect triggered for bindgroup");
                } else {
                    ++it;
                }
            }
        }
    }

    wgpu::BindGroup get(const BindGroupCacheKey<N>& key) {
        auto it = map_.find(key);
        if (it != map_.end()) {
            // Cache Hit: Update the frame counter to keep it alive
            it->second.last_used_frame = current_frame_;
            return it->second.bindgroup;
        }

        std::vector<wgpu::BindGroupEntry> entries;
        entries.reserve(N);

        for (uint32_t i = 0; i < N; ++i) {
            core::Handle<Texture> handle = key.textures[i];

            // If the handle is invalid (index 0, gen 0), this slot is empty. Skip it.
            if (!handle.valid()) {
                continue;
            }

            Texture& texture = context_->device->get_texture(handle);

            wgpu::BindGroupEntry entry{};
            entry.binding = i;
            entry.textureView = texture.view;

            entries.push_back(entry);
        }

        wgpu::BindGroupDescriptor desc{};

        desc.layout = bindgroup_layout_;

        desc.entryCount = static_cast<uint32_t>(entries.size());
        desc.entries = entries.data();

        wgpu::BindGroup new_bindgroup = context_->device->device().CreateBindGroup(&desc);
        map_[key] = {new_bindgroup, current_frame_};
        return new_bindgroup;
    }

private:
    app::ApplicationContext* context_;
    std::unordered_map<BindGroupCacheKey<N>, CachedBindGroup, BindGroupCacheKeyHash<N>> map_;
    wgpu::BindGroupLayout bindgroup_layout_;
    uint64_t current_frame_{0};
};

}  // namespace bvr::gfx
