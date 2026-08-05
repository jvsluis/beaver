#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "beaver/app/application.h"
#include "beaver/app/enginecontext.h"
#include "beaver/app/entry.h"
#include "beaver/graphics/gpuresources.h"
#include "beaver/graphics/renderer.h"
#include "glm/ext/matrix_float4x4.hpp"

class GameLayer : public bvr::app::Layer {
public:
    GameLayer() {}

    void on_attach(const bvr::app::EngineContext& context) override {
        framebuffer_ = context.device.create_framebuffer(680, 400);

        view_.colour_target = framebuffer_;
        view_.viewport = {0, 0, 680, 200};
        view_.uniforms.projection_matrix = glm::ortho(0.0f, 680.0f, 200.0f, 0.0f, -1.0f, 1.0f);
        view_.uniforms.view_matrix = glm::mat4x4(1.0);

        spritesheet_ = context.asset_manager.load_texture("../assets/spritesheet.png");
    }

    void on_render(bvr::graphics::Renderer& renderer) override {
        // Draw sub texture at 4:1 size
        renderer.draw_textured_rect(spritesheet_, {16, 16, 64, 64}, {0.0, 0.0, 0.5, 0.5});

        // Draw sub texture at 1:1 size
        renderer.draw_textured_rect(spritesheet_, {100, 50, 16, 16}, {0.5, 0.0, 0.5, 0.5});

        // Demonstrate texture recolouring with 2:1
        renderer.draw_textured_rect(spritesheet_, {100, 125, 32, 32}, {0.5, 0.0, 0.5, 0.5}, {20, 20, 255, 255});

        renderer.flush_2d(view_, true);
        renderer.blit_to_surface(view_.colour_target);
    }

private:
    bvr::graphics::RenderView view_;
    bvr::graphics::RenderView view2_;
    bvr::core::Handle<bvr::graphics::Texture> framebuffer_;
    bvr::core::Handle<bvr::graphics::Texture> spritesheet_;
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
