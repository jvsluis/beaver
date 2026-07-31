#include "beaver/app/application.h"

#include <ranges>

#include "beaver/core/log.h"

namespace bvr::app {

Application::Application(const ApplicationDescriptor& desc) {
    running_ = false;

    context_.window = std::make_unique<Window>();
    context_.window->create(desc.window_desc);

    context_.device = std::make_unique<gfx::Device>();
    context_.device->create(context_.window.get(), desc.vsync);

    context_.asset_manager = std::make_unique<asset::AssetManager>();
    context_.asset_manager->create(*context_.device);

    renderer_.create(context_);

    CORE_INFO("Finished creating the application");
}

Application::~Application() {
    for (auto& layer : std::views::reverse(layers_)) {
        layer->on_detach();
        layer = nullptr;
    }

    context_.device->destroy();
    context_.device.reset();

    context_.window->destroy();
    context_.window.reset();
}

void Application::run() {
    running_ = true;

    auto engine_start_time = std::chrono::high_resolution_clock::now();
    auto last_frame_time = engine_start_time;

    while (running_) {
        auto current_time = std::chrono::high_resolution_clock::now();
        double delta_time = std::chrono::duration<double>(current_time - last_frame_time).count();
        last_frame_time = current_time;

        context_.window->update();
        main_thread_queue_.process();

        renderer_.start_frame();

        for (Layer* layer : layers_) {
            if (layer->is_active()) {
                layer->on_update(delta_time);
            }
        }

        for (Layer* layer : layers_) {
            if (layer->is_active()) {
                layer->on_render(renderer_);
            }
        }

        renderer_.end_frame();

        context_.device->surface_present();
        context_.device->tick();

        if (context_.window->should_close()) {
            stop();
        }
    }
}

void Application::stop() {
    running_ = false;
}

}  // namespace bvr::app
