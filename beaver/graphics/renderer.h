#pragma once

#include "beaver/graphics/bindgroupcache.h"
#include "beaver/graphics/gpuresources.h"
#include "beaver/graphics/renderer2d.h"
#include "webgpu/webgpu_cpp.h"

namespace bvr::graphics {

class Renderer {
public:
    Renderer(Device& device) : device_(device), renderer2d_(device) {}
    ~Renderer() = default;

    void create();
    void destroy();
    void start_frame();
    void end_frame();
    void flush_2d(std::span<RenderView*> views);
    void run_gc();

    void blit_to_surface(core::Handle<Texture> handle);

    inline void draw_textured_rect(core::Handle<Texture> handle, core::Rect<uint16_t> position, core::Rect<float> uv, core::Colour<uint8_t> colour = {255, 255, 255, 255}) {
        renderer2d_.draw_textured_rect(handle, position, uv, colour);
    }

    wgpu::TextureView& surface_view() {
        return surface_view_;
    }

private:
    Device& device_;

    wgpu::TextureView surface_view_;
    wgpu::CommandEncoder current_encoder_;
    Renderer2D renderer2d_;

    wgpu::RenderPipeline blit_render_pipeline_;
    wgpu::Sampler blit_sampler_;
    wgpu::BindGroup blit_sampler_bindgroup_;
    BindGroupCache<1> blit_bindgroup_cache_;
};

}  // namespace bvr::graphics
