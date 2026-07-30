#pragma once

#include <webgpu/webgpu_cpp.h>

#include <cstdint>

namespace bvr::gfx {

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

class Texture {
    wgpu::Texture texture;
    wgpu::TextureView view;
    wgpu::TextureUsage usage;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
};

class Buffer {
    wgpu::Buffer buffer;
    uint64_t size;
};

class Sampler {
    wgpu::Sampler sampler;
};

}  // namespace bvr::gfx
