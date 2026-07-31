#pragma once

#include <webgpu/webgpu_cpp.h>

#include "beaver/app/window.h"
#include "beaver/graphics/gpuresources.h"
#include "beaver/graphics/resourcepool.h"

namespace bvr::gfx {

class Device {
public:
    Device() = default;
    ~Device() = default;

    void create(app::Window* window, bool vsync);
    void destroy();
    void surface_present();
    void tick();

    wgpu::TextureView get_surface_texture_view();

    Handle<Buffer> create_buffer();
    Handle<Buffer> destroy_buffer();

    Handle<Texture> create_texture(const TextureDescriptor& desc);
    void write_texture(Handle<Texture> handle, void* data, uint32_t size);
    Texture& get_texture(Handle<Texture> handle);
    void destroy_texture(Handle<Texture> handle);
    Handle<Texture> create_framebuffer(uint32_t width, uint32_t height);

    wgpu::Device& device() { return device_; }
    wgpu::Queue& queue() { return queue_; }

private:
    void setup_device();
    void configure_surface();

    app::Window* window_{nullptr};
    bool vsync_{false};

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

    Handle<Texture> white_pixel_;
    Handle<Texture> magenta_pixel_;

    ResourcePool<Texture> texture_pool_;
};

}  // namespace bvr::gfx
