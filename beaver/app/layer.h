#pragma once

#include "beaver/app/applicationcontext.h"
#include "beaver/events/event.h"
#include "beaver/graphics/renderer.h"

namespace bvr::app {

class Layer {
public:
    Layer() = default;
    ~Layer() = default;

    virtual void on_attach(ApplicationContext& context) {
        context_ = &context;
    }

    virtual void on_detach() {}

    virtual void on_event(event::Event& event) {}

    virtual void on_update(double delta) {}

    virtual void on_render(gfx::Renderer& renderer) {}

private:
    ApplicationContext* context_;
};

}  // namespace bvr::app
