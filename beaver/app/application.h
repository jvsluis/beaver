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
        layer->on_attach(context_);
    }

    static void submit_to_main_thread(std::function<void()> task) {
        s_instance->main_thread_queue_.submit(task);
    }

private:
    static Application* s_instance;

    bool running_;
    ApplicationContext context_;
    core::TaskQueue main_thread_queue_;
    gfx::Renderer renderer_;
    std::vector<Layer*> layers_;
};

std::unique_ptr<Application> create_application(CommandLineArgs& args);

}  // namespace bvr::app
