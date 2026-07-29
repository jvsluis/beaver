#pragma once

#include <webgpu/webgpu_cpp.h>

namespace bvr::gfx {

class Device {
public:
    Device() = default;
    ~Device() = default;

    void create();
    void destroy();

private:
    wgpu::Instance instance_;
    wgpu::Adapter adapter_;
    wgpu::Device device_;
    wgpu::Queue queue_;

    wgpu::Surface surface_;
    wgpu::TextureFormat surface_format_;
    wgpu::SurfaceTexture current_surface_texture_;
};

}  // namespace bvr::gfx
