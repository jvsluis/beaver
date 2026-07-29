#include "beaver/app/application.h"

namespace bvr::app {

Application::Application(const ApplicationDescriptor& desc) {
    running_ = false;

    context_.window = std::make_unique<Window>();
    context_.window->create(desc.window_desc);

    context_.device = std::make_unique<gfx::Device>();
    context_.device->create();

    renderer_.create();
}

Application::~Application() {
    context_.device->destroy();
    context_.device.reset();

    context_.window->destroy();
    context_.window.reset();
}

void Application::run() {
    running_ = true;

    while (running_) {
        main_thread_queue_.process();

        if (context_.window->should_close()) {
            stop();
        }
    }
}

void Application::stop() {
    running_ = false;
}

}  // namespace bvr::app
