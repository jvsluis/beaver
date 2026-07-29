#pragma once

#include <webgpu/webgpu_cpp.h>

#include "beaver/core/colour.h"
#include "beaver/core/geometry.h"
#include "beaver/graphics/texturepool.h"

namespace bvr::gfx {

class Renderer2D {
public:
    Renderer2D() = default;
    ~Renderer2D() = default;

    void create();
    void destroy();

    void start_frame();
    void end_frame();

    void draw_textured_rect(TextureHandle handle, core::Rect<uint16_t> position, core::Rect<float> uv, core::Colour<uint8_t> colour = {255, 255, 255, 255});

    void flush(wgpu::TextureView target);

private:
};

}  // namespace bvr::gfx
