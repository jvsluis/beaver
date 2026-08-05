#pragma once

#include <webgpu/webgpu_cpp.h>

#include "beaver/core/colour.h"
#include "beaver/core/geometry.h"
#include "beaver/graphics/bindgroupcache.h"
#include "beaver/graphics/gpuresources.h"
#include "beaver/graphics/renderview.h"

namespace bvr::gfx {

constexpr size_t MAX_TEXTURE_SLOTS = 4;

class Renderer2D {
public:
    Renderer2D(Device& device) : device_(device) {}
    ~Renderer2D() = default;

    void create();
    void destroy();
    void start_frame(wgpu::CommandEncoder& encoder);
    void end_frame();

    void draw_textured_rect(core::Handle<Texture> handle, core::Rect<uint16_t> position, core::Rect<float> uv, core::Colour<uint8_t> colour = {255, 255, 255, 255});

    void flush(RenderView& view, bool clear_background);

private:
    Device& device_;

    struct RenderCommand2D {
        uint32_t texture_id;
        core::Rect<uint16_t> position;
        core::Rect<float> uv;
        core::Colour<uint8_t> colour;
    };

    struct RenderBatch2D {
        BindGroupCacheKey<MAX_TEXTURE_SLOTS> key;
        uint32_t size = 0;
    };

    std::vector<RenderCommand2D> commands_;
    std::vector<RenderBatch2D> batches_;

    wgpu::CommandEncoder current_command_encoder_;
    wgpu::RenderPipeline render_pipeline_;
    wgpu::Sampler sampler_;
    wgpu::BindGroup render_bindgroup_;

    BindGroupCache<MAX_TEXTURE_SLOTS> render_bindgroup_cache_;
    BindGroupCacheKey<MAX_TEXTURE_SLOTS> active_textures_;
    uint32_t texture_count_ = 0;
    uint32_t batch_count_ = 0;

    core::Handle<Texture> white_pixel_;
    core::Handle<Buffer> uniforms_buffer_;
    core::Handle<Buffer> command_buffer_;
};

}  // namespace bvr::gfx
