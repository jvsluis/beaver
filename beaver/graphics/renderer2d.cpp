#include "beaver/graphics/renderer2d.h"

namespace bvr::gfx {

void Renderer2D::create() {}

void Renderer2D::destroy() {}

void Renderer2D::draw_textured_rect(TextureHandle handle, core::Rect<uint16_t> position, core::Rect<float> uv, core::Colour<uint8_t> colour) {
}

void Renderer2D::flush(wgpu::TextureView target) {}

}  // namespace bvr::gfx
