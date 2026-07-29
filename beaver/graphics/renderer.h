#pragma once

#include "beaver/graphics/renderer2d.h"
#include "webgpu/webgpu_cpp.h"

namespace bvr::gfx {

class Renderer {
public:
    Renderer() = default;
    ~Renderer() = default;

    void create();
    void destroy();

    void start_frame();
    void end_frame();

    inline void draw_textured_rect(TextureHandle handle, core::Rect<uint16_t> position, core::Rect<float> uv, core::Colour<uint8_t> colour = {255, 255, 255, 255}) {
        renderer2d_.draw_textured_rect(handle, position, uv, colour);
    }

private:
    wgpu::CommandEncoder current_encoder_;
    Renderer2D renderer2d_;
};

}  // namespace bvr::gfx
