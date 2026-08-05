#pragma once

#include <memory>

#include "beaver/app/applicationcontext.h"
#include "beaver/app/layer.h"
#include "beaver/app/window.h"
#include "beaver/core/taskqueue.h"
#include "beaver/graphics/renderer.h"

namespace bvr::app {

struct CommandLineArgs {
    int count;
    char** args;
};

struct ApplicationDescriptor {
    WindowDescriptor window_desc;
    bool vsync = false;
};

class Application {
public:
    Application(const ApplicationDescriptor& desc);
    ~Application();

    void run();
    void stop();

    void add_layer(Layer* layer) {
        layers_.push_back(layer);
        layer->on_attach(engine_context());
    }

    EngineContext engine_context() {
        return EngineContext{
            .window = *window_,
            .device = *device_,
            .asset_manager = *asset_manager_,
            .main_thread_queue = *main_thread_queue_,
        };
    }

private:
    static Application* s_instance;

    bool running_;

    std::unique_ptr<Window> window_;
    std::unique_ptr<gfx::Device> device_;
    std::unique_ptr<asset::AssetManager> asset_manager_;
    std::unique_ptr<core::TaskQueue> main_thread_queue_;
    std::unique_ptr<gfx::Renderer> renderer_;

    std::vector<Layer*> layers_;
};

std::unique_ptr<Application> create_application(CommandLineArgs& args);

}  // namespace bvr::app
