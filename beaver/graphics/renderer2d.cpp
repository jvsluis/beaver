#include "beaver/graphics/renderer2d.h"

#include "beaver/app/applicationcontext.h"
#include "beaver/graphics/shaders/render2d.wgsl.h"
#include "webgpu/webgpu_cpp.h"

namespace bvr::gfx {

void Renderer2D::create(app::ApplicationContext& context) {
    context_ = &context;

    // Init the pipeline
    wgpu::ShaderSourceWGSL wgsl{{.code = render2d_wgsl}};

    wgpu::ShaderModuleDescriptor shaderModuleDescriptor{.nextInChain = &wgsl};
    wgpu::ShaderModule shaderModule = context_->device->device().CreateShaderModule(&shaderModuleDescriptor);

    wgpu::ColorTargetState colorTargetState{.format = wgpu::TextureFormat::RGBA8Unorm};

    wgpu::FragmentState fragmentState{
        .module = shaderModule,
        .targetCount = 1,
        .targets = &colorTargetState,
    };

    wgpu::RenderPipelineDescriptor descriptor{
        .vertex = {
            .module = shaderModule,
        },
        .fragment = &fragmentState,
    };
    render_pipeline_ = context_->device->device().CreateRenderPipeline(&descriptor);
}

void Renderer2D::destroy() {}

void Renderer2D::start_frame(wgpu::CommandEncoder& encoder) {
    current_command_encoder_ = encoder;
}

void Renderer2D::end_frame() {
    current_command_encoder_ = nullptr;
}

void Renderer2D::draw_textured_rect(Handle<Texture> handle, core::Rect<uint16_t> position, core::Rect<float> uv, core::Colour<uint8_t> colour) {
    // TODO(jvsluis): implement
}

void Renderer2D::flush(RenderView& view, bool clear_background) {
    wgpu::TextureView texture_view = context_->device->get_texture(view.colour_target).view;

    wgpu::RenderPassColorAttachment attachment{
        .view = texture_view,
        .loadOp = clear_background ? wgpu::LoadOp::Clear : wgpu::LoadOp::Load,
        .storeOp = wgpu::StoreOp::Store,
        .clearValue = {0.0, 0.0, 1.0, 1.0},
    };

    wgpu::RenderPassDescriptor renderpass{.colorAttachmentCount = 1,
                                          .colorAttachments = &attachment};

    wgpu::RenderPassEncoder pass = current_command_encoder_.BeginRenderPass(&renderpass);

    pass.SetViewport(
        view.viewport.x, view.viewport.y,
        view.viewport.width, view.viewport.height,
        0.0, 1.0);

    pass.SetPipeline(render_pipeline_);
    pass.Draw(3);
    pass.End();
}

}  // namespace bvr::gfx
