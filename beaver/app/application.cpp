#include "beaver/app/application.h"

#include <memory>
#include <ranges>

#include "beaver/core/log.h"
#include "beaver/core/taskqueue.h"

namespace bvr::app {

Application::Application(const ApplicationDescriptor& desc) {
    running_ = false;

    window_ = std::make_unique<Window>();
    window_->create(desc.window_desc);

    device_ = std::make_unique<graphics::Device>();
    device_->create(window_.get(), desc.vsync);

    asset_manager_ = std::make_unique<assets::AssetManager>();
    asset_manager_->create(*device_);

    main_thread_queue_ = std::make_unique<core::TaskQueue>();

    renderer_ = std::make_unique<graphics::Renderer>(*device_);
    renderer_->create();

    CORE_INFO("Finished creating the application");
}

Application::~Application() {
    for (auto& layer : std::views::reverse(layers_)) {
        layer->on_detach();
        layer = nullptr;
    }

    device_->destroy();
    device_.reset();

    window_->destroy();
    window_.reset();
}

void Application::run() {
    running_ = true;

    auto engine_start_time = std::chrono::high_resolution_clock::now();
    auto last_frame_time = engine_start_time;

    while (running_) {
        auto current_time = std::chrono::high_resolution_clock::now();
        double delta_time = std::chrono::duration<double>(current_time - last_frame_time).count();
        last_frame_time = current_time;

        window_->update();
        main_thread_queue_->process();

        renderer_->start_frame();

        for (Layer* layer : layers_) {
            if (layer->is_active()) {
                layer->on_update(delta_time);
            }
        }

        for (Layer* layer : layers_) {
            if (layer->is_active()) {
                layer->on_render(*renderer_);
            }
        }

        renderer_->end_frame();

        device_->surface_present();
        device_->tick();

        if (window_->should_close()) {
            stop();
        }
    }
}

void Application::stop() {
    running_ = false;
}

}  // namespace bvr::app
