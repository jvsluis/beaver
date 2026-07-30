#include "beaver/graphics/device.h"

#include <webgpu/webgpu_glfw.h>

#include <cstdlib>

#include "beaver/core/log.h"
#include "webgpu/webgpu_cpp.h"

namespace bvr::gfx {

void Device::create(app::Window* window) {
    window_ = window;
    setup_device();
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

    wgpu::SurfaceConfiguration config{.device = device_,
                                      .format = surface_format_,
                                      .width = surface_width_,
                                      .height = surface_height_};
    surface_.Configure(&config);
}

}  // namespace bvr::gfx
