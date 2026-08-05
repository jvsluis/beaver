#pragma once

#include "beaver/app/applicationcontext.h"
#include "beaver/events/event.h"
#include "beaver/graphics/renderer.h"

namespace bvr::app {

class Layer {
public:
    Layer() = default;
    ~Layer() = default;

    virtual void on_attach(const EngineContext& context) {
        context_ = &context;
    }

    virtual void on_detach() {}

    virtual void on_event(event::Event& event) {}

    virtual void on_update(double delta) {}

    virtual void on_render(gfx::Renderer& renderer) {}

    bool is_active() {
        return is_active_;
    }

    void set_active(bool active) {
        is_active_ = active;
    }

private:
    const EngineContext* context_;
    bool is_active_{true};
};

}  // namespace bvr::app
