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

struct TextureDescriptor {
    const char* label;
    uint32_t width;
    uint32_t height;
    wgpu::TextureUsage usage = wgpu::TextureUsage::RenderAttachment;
    wgpu::TextureFormat format = wgpu::TextureFormat::RGBA8Unorm;
};

struct Texture {
    wgpu::Texture texture;
    wgpu::TextureView view;
    wgpu::TextureFormat format;
    uint32_t width;
    uint32_t height;
};

class Buffer {
    wgpu::Buffer buffer;
    uint64_t size;
};

class Sampler {
    wgpu::Sampler sampler;
};

}  // namespace bvr::gfx
