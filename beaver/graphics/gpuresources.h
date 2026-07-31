#pragma once

#include <webgpu/webgpu_cpp.h>

#include <cstdint>

namespace bvr::gfx {

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
