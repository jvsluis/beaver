#include <memory>

#include "beaver/app/application.h"
#include "beaver/app/applicationcontext.h"
#include "beaver/app/entry.h"
#include "beaver/graphics/gpuresources.h"
#include "beaver/graphics/renderer.h"

class GameLayer : public bvr::app::Layer {
public:
    GameLayer() {}

    void on_attach(bvr::app::ApplicationContext& context) override {
        framebuffer_ = context.device->create_framebuffer(680, 400);

        view_.colour_target = framebuffer_;
        view_.viewport = {0, 0, 680, 400};
    }

    void on_render(bvr::gfx::Renderer& renderer) override {
        renderer.flush_2d(view_, true);
        renderer.blit_to_surface(view_.colour_target);
    }

private:
    bvr::gfx::RenderView view_;
    bvr::gfx::Handle<bvr::gfx::Texture> framebuffer_;
};

class BasicApplication : public bvr::app::Application {
public:
    BasicApplication(const bvr::app::ApplicationDescriptor& desc) : bvr::app::Application(desc) {
        add_layer(new GameLayer());
    }
};

std::unique_ptr<bvr::app::Application> bvr::app::create_application(bvr::app::CommandLineArgs& args) {
    bvr::app::ApplicationDescriptor desc = {
        .window_desc = {
            .title = "Basic Application",
            .width = 680,
            .height = 400,
            .resizable = false,
        },
        .vsync = true,
    };

    return std::make_unique<BasicApplication>(desc);
}
