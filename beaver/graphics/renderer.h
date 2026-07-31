#pragma once

#include "beaver/app/applicationcontext.h"
#include "beaver/graphics/bindgroupcache.h"
#include "beaver/graphics/gpuresources.h"
#include "beaver/graphics/renderer2d.h"
#include "webgpu/webgpu_cpp.h"

namespace bvr::gfx {

class Renderer {
public:
    Renderer() = default;
    ~Renderer() = default;

    void create(app::ApplicationContext& context);
    void destroy();
    void start_frame();
    void end_frame();
    void flush_2d(RenderView& view, bool clear_background);
    void run_gc();

    void blit_to_surface(Handle<Texture> handle);

    inline void draw_textured_rect(Handle<Texture> handle, core::Rect<uint16_t> position, core::Rect<float> uv, core::Colour<uint8_t> colour = {255, 255, 255, 255}) {
        renderer2d_.draw_textured_rect(handle, position, uv, colour);
    }

    wgpu::TextureView& surface_view() {
        return surface_view_;
    }

private:
    app::ApplicationContext* context_{nullptr};
    wgpu::TextureView surface_view_;
    wgpu::CommandEncoder current_encoder_;
    Renderer2D renderer2d_;

    wgpu::RenderPipeline blit_render_pipeline_;
    wgpu::Sampler blit_sampler_;
    wgpu::BindGroup blit_sampler_bindgroup_;
    BindGroupCache<1> blit_bindgroup_cache_;
};

}  // namespace bvr::gfx
