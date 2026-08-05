#pragma once

#include <webgpu/webgpu_cpp.h>

#include "beaver/app/window.h"
#include "beaver/core/handle.h"
#include "beaver/graphics/gpuresources.h"
#include "beaver/graphics/resourcepool.h"

namespace bvr::graphics {

class Device {
public:
    Device() = default;
    ~Device() = default;

    void create(app::Window* window, bool vsync);
    void destroy();
    void surface_present();
    void tick();

    wgpu::TextureView get_surface_texture_view();

    core::Handle<Buffer> create_buffer(const BufferDescriptor& desc);
    void write_buffer(core::Handle<Buffer> handle, void* data, uint64_t offset, uint64_t size);
    Buffer& get_buffer(core::Handle<Buffer> handle);
    void destroy_buffer(core::Handle<Buffer> handle);

    core::Handle<Texture> create_texture(const TextureDescriptor& desc);
    void write_texture(core::Handle<Texture> handle, void* data, uint32_t size);
    Texture& get_texture(core::Handle<Texture> handle);
    void destroy_texture(core::Handle<Texture> handle);
    core::Handle<Texture> create_framebuffer(uint32_t width, uint32_t height);

    wgpu::Device& device() { return device_; }
    wgpu::Queue& queue() { return queue_; }
    wgpu::TextureFormat surface_format() { return surface_format_; }
    core::Handle<Texture> white_pixel() { return white_pixel_; };
    core::Handle<Texture> magenta_pixel() { return magenta_pixel_; }

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

    core::Handle<Texture> white_pixel_;
    core::Handle<Texture> magenta_pixel_;
    core::Handle<Buffer> dummy_buffer_;

    ResourcePool<Buffer> buffer_pool_;
    ResourcePool<Texture> texture_pool_;
};

}  // namespace bvr::graphics
