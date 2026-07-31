#include "beaver/graphics/device.h"

#include <webgpu/webgpu_glfw.h>

#include <cstdlib>

#include "beaver/app/window.h"
#include "beaver/core/log.h"
#include "beaver/graphics/gpuresources.h"
#include "webgpu/webgpu_cpp.h"

namespace bvr::gfx {

void Device::create(app::Window* window, bool vsync) {
    window_ = window;
    vsync_ = vsync;

    app::WindowSize window_size = window->window_size();
    surface_width_ = window_size.framebuffer_width;
    surface_height_ = window_size.framebuffer_height;

    setup_device();

    // Create the default textures
    TextureDescriptor desc{};
    desc.width = 1;
    desc.height = 1;
    desc.label = "default1x1";
    desc.usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst;
    desc.format = wgpu::TextureFormat::RGBA8Unorm;

    white_pixel_ = create_texture(desc);
    magenta_pixel_ = create_texture(desc);

    uint8_t white_pixel[4] = {255, 255, 255, 255};
    write_texture(white_pixel_, &white_pixel, 4);

    uint8_t magenta_pixel[4] = {255, 0, 255, 255};
    write_texture(magenta_pixel_, &magenta_pixel, 4);

    // Create the dummy buffer
    BufferDescriptor buffer_desc{};
    buffer_desc.label = "default buffer";
    buffer_desc.size = 1;
    buffer_desc.usage = wgpu::BufferUsage::CopyDst;
    dummy_buffer_ = create_buffer(buffer_desc);

    uint8_t zero = 0;
    write_buffer(dummy_buffer_, &zero, 0, 1);
}

void Device::destroy() {}

void Device::surface_present() {
    surface_.Present();
}

void Device::tick() {
    instance_.ProcessEvents();
}

wgpu::TextureView Device::get_surface_texture_view() {
    if (is_surface_dirty_) {
        surface_.Unconfigure();
        configure_surface();
    }

    surface_.GetCurrentTexture(&current_surface_texture_);
    return current_surface_texture_.texture.CreateView();
}

core::Handle<Buffer> Device::create_buffer(const BufferDescriptor& desc) {
    wgpu::BufferDescriptor bdesc = {
        .label = desc.label,
        .usage = desc.usage,
        .size = desc.size,
        .mappedAtCreation = false,
    };

    wgpu::Buffer raw_buffer = device_.CreateBuffer(&bdesc);

    Buffer buffer;
    buffer.buffer = raw_buffer;
    buffer.size = desc.size;

    return buffer_pool_.insert(buffer);
}

void Device::write_buffer(core::Handle<Buffer> handle, void* data, uint64_t offset, uint64_t size) {
    Buffer* buffer = buffer_pool_.get(handle);

    if (!buffer) {
        CORE_ERROR("Attempted to write a buffer with an invalid handle");
        return;
    }

    queue_.WriteBuffer(buffer->buffer, offset, data, size);
}

Buffer& Device::get_buffer(core::Handle<Buffer> handle) {
    Buffer* buffer = buffer_pool_.get(handle);

    if (!buffer) {
        CORE_ERROR("Attempted to get a buffer with an invalid handle");

        // Return the dummy buffer to try prevent application from crashing
        return *buffer_pool_.get(dummy_buffer_);
    }

    return *buffer;
}

void Device::destroy_buffer(core::Handle<Buffer> handle) {
    buffer_pool_.remove(handle);
}

core::Handle<Texture> Device::create_texture(const TextureDescriptor& desc) {
    wgpu::TextureDescriptor tdesc = {
        .label = desc.label,
        .usage = desc.usage,
        .dimension = wgpu::TextureDimension::e2D,
        .size = {desc.width, desc.height, 1},
        .format = desc.format,
        .mipLevelCount = 1,
        .sampleCount = 1,
    };

    wgpu::Texture raw_texture = device_.CreateTexture(&tdesc);

    wgpu::TextureViewDescriptor view_desc{};
    view_desc.format = desc.format;
    view_desc.dimension = wgpu::TextureViewDimension::e2D;
    view_desc.baseMipLevel = 0;
    view_desc.mipLevelCount = 1;
    view_desc.baseArrayLayer = 0;
    view_desc.arrayLayerCount = 1;

    wgpu::TextureView raw_view = raw_texture.CreateView(&view_desc);

    Texture tex;
    tex.texture = raw_texture;
    tex.view = raw_view;
    tex.format = desc.format;
    tex.width = desc.width;
    tex.height = desc.height;

    return texture_pool_.insert(tex);
}

void Device::write_texture(core::Handle<Texture> handle, void* data, uint32_t size) {
    Texture* tex = texture_pool_.get(handle);

    if (!tex) {
        CORE_ERROR("Attempted to write a texture with an invalid handle");
        return;
    }

    wgpu::TexelCopyTextureInfo destination{};
    destination.texture = tex->texture;

    wgpu::TexelCopyBufferLayout dataLayout{};
    // Assuming 4 bytes per pixel (RGBA8).
    // TODO(jvsluis): compute this based on the format
    uint32_t bytesPerPixel = 4;
    dataLayout.bytesPerRow = tex->width * bytesPerPixel;
    dataLayout.rowsPerImage = tex->height;

    wgpu::Extent3D extent = {tex->width, tex->height, 1};
    queue_.WriteTexture(&destination, data, size, &dataLayout, &extent);
}

Texture& Device::get_texture(core::Handle<Texture> handle) {
    Texture* tex = texture_pool_.get(handle);

    if (!tex) {
        CORE_ERROR("Attempted to get a texture with an invalid handle");
        return *texture_pool_.get(magenta_pixel_);
    }

    return *tex;
}

void Device::destroy_texture(core::Handle<Texture> handle) {
    texture_pool_.remove(handle);
}

core::Handle<Texture> Device::create_framebuffer(uint32_t width, uint32_t height) {
    bvr::gfx::TextureDescriptor desc = {
        .label = "FrameBuffer",
        .width = width,
        .height = height,
        .usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding,
        .format = wgpu::TextureFormat::RGBA8Unorm,
    };
    return create_texture(desc);
}

void Device::setup_device() {
    static const auto kTimedWaitAny = wgpu::InstanceFeatureName::TimedWaitAny;
    wgpu::InstanceDescriptor instanceDesc{.requiredFeatureCount = 1,
                                          .requiredFeatures = &kTimedWaitAny};
    instance_ = wgpu::CreateInstance(&instanceDesc);

    if (!instance_) {
        CORE_FATAL("Unable to create wgpu instance!");
        std::abort();
    }

    // Create the surface
    surface_ = wgpu::glfw::CreateSurfaceForWindow(instance_, window_->handle());

    if (!surface_) {
        CORE_FATAL("Unable to create wgpu surface for window!");
        std::abort();
    }

    wgpu::Future f1 = instance_.RequestAdapter(
        nullptr, wgpu::CallbackMode::WaitAnyOnly,
        [this](wgpu::RequestAdapterStatus status, wgpu::Adapter a,
               wgpu::StringView message) {
            if (status != wgpu::RequestAdapterStatus::Success) {
                CORE_ERROR("Request Adapter: %s\n", message);
                exit(0);
            }
            adapter_ = std::move(a);
        });
    instance_.WaitAny(f1, UINT64_MAX);

    if (!adapter_) {
        CORE_FATAL("Unable to acquire adapter!");
        std::abort();
    }

    wgpu::DeviceDescriptor desc{};
    desc.SetUncapturedErrorCallback([](const wgpu::Device&,
                                       wgpu::ErrorType error_type,
                                       wgpu::StringView message) {
        CORE_ERROR("WGPU Error: %d - %s\n", error_type, message);
    });
    desc.SetDeviceLostCallback(wgpu::CallbackMode::WaitAnyOnly, [](const wgpu::Device&, wgpu::DeviceLostReason reason, wgpu::StringView message) {
        CORE_ERROR("WGPU Device Lost Error: %d - %s\n", reason, message);
    });

    wgpu::Future f2 = adapter_.RequestDevice(
        &desc, wgpu::CallbackMode::WaitAnyOnly,
        [this](wgpu::RequestDeviceStatus status, wgpu::Device d,
               wgpu::StringView message) {
            if (status != wgpu::RequestDeviceStatus::Success) {
                CORE_ERROR("Request Device: %s\n", message);
                exit(0);
            }
            device_ = std::move(d);
        });
    instance_.WaitAny(f2, UINT64_MAX);

    if (!device_) {
        CORE_FATAL("Unable to acquire device!");
        std::abort();
    }

    queue_ = device_.GetQueue();
    configure_surface();
}

void Device::configure_surface() {
    wgpu::SurfaceCapabilities capabilities;
    surface_.GetCapabilities(adapter_, &capabilities);
    surface_format_ = capabilities.formats[0];

    wgpu::PresentMode present_mode = wgpu::PresentMode::Immediate;
    if (vsync_) {
        CORE_INFO("Enabled VSync");
        present_mode = wgpu::PresentMode::Fifo;
    }

    wgpu::SurfaceConfiguration config{.device = device_,
                                      .format = surface_format_,
                                      .width = surface_width_,
                                      .height = surface_height_,
                                      .presentMode = present_mode};
    surface_.Configure(&config);
    is_surface_dirty_ = false;
}

}  // namespace bvr::gfx
