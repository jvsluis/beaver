#pragma once

#include <webgpu/webgpu_cpp.h>

#include "beaver/app/window.h"
#include "beaver/graphics/gpuresources.h"

namespace bvr::gfx {

class Device {
public:
    Device() = default;
    ~Device() = default;

    void create(app::Window* window);
    void destroy();
    void surface_present();
    void tick();

    wgpu::TextureView get_surface_texture_view();

    Handle<Buffer> create_buffer();
    Handle<Buffer> destroy_buffer();

    Handle<Texture> create_texture();
    Handle<Texture> destroy_texture();

private:
    void setup_device();
    void configure_surface();

    app::Window* window_;

    wgpu::Instance instance_{nullptr};
    wgpu::Adapter adapter_{nullptr};
    wgpu::Device device_{nullptr};
    wgpu::Queue queue_{nullptr};

    wgpu::Surface surface_{nullptr};
    wgpu::TextureFormat surface_format_{wgpu::TextureFormat::Undefined};
    wgpu::SurfaceTexture current_surface_texture_{nullptr};
    bool is_surface_dirty_{true};
    uint32_t surface_width_{1};
    uint32_t surface_height_{1};
};

}  // namespace bvr::gfx
