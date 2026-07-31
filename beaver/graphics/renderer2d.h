#pragma once

#include <webgpu/webgpu_cpp.h>

#include "beaver/app/applicationcontext.h"
#include "beaver/core/colour.h"
#include "beaver/core/geometry.h"
#include "beaver/graphics/gpuresources.h"
#include "beaver/graphics/renderview.h"

namespace bvr::gfx {

class Renderer2D {
public:
    Renderer2D() = default;
    ~Renderer2D() = default;

    void create(app::ApplicationContext& context);
    void destroy();
    void start_frame(wgpu::CommandEncoder& encoder);
    void end_frame();

    void draw_textured_rect(core::Handle<Texture> handle, core::Rect<uint16_t> position, core::Rect<float> uv, core::Colour<uint8_t> colour = {255, 255, 255, 255});

    void flush(RenderView& view, bool clear_background);

private:
    app::ApplicationContext* context_;
    wgpu::CommandEncoder current_command_encoder_;

    wgpu::RenderPipeline render_pipeline_;
};

}  // namespace bvr::gfx
