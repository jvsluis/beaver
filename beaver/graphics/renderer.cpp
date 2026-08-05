#include "beaver/graphics/renderer.h"

#include "beaver/graphics/shaders/blit.wgsl.h"

namespace bvr::graphics {

void Renderer::create() {
    renderer2d_.create();

    // Create the blit pipeline
    wgpu::Device& device = device_.device();

    wgpu::ShaderSourceWGSL wgsl{{.code = blit_wgsl}};

    wgpu::ShaderModuleDescriptor shaderModuleDescriptor{.nextInChain = &wgsl};
    wgpu::ShaderModule shaderModule = device.CreateShaderModule(&shaderModuleDescriptor);

    wgpu::ColorTargetState colorTargetState{.format = device_.surface_format()};

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
    blit_render_pipeline_ = device.CreateRenderPipeline(&descriptor);

    // Create the sampler
    wgpu::SamplerDescriptor samplerDesc;
    samplerDesc.magFilter = wgpu::FilterMode::Linear;
    samplerDesc.minFilter = wgpu::FilterMode::Linear;
    samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Linear;
    // Clamp to edge prevents weird wrapping artifacts at the borders
    samplerDesc.addressModeU = wgpu::AddressMode::ClampToEdge;
    samplerDesc.addressModeV = wgpu::AddressMode::ClampToEdge;
    samplerDesc.addressModeW = wgpu::AddressMode::ClampToEdge;
    blit_sampler_ = device.CreateSampler(&samplerDesc);

    wgpu::BindGroupEntry bg_entries[1] = {};
    bg_entries[0].binding = 0;
    bg_entries[0].sampler = blit_sampler_;

    wgpu::BindGroupDescriptor bg_desc;
    bg_desc.layout = blit_render_pipeline_.GetBindGroupLayout(0);
    bg_desc.entryCount = 1;
    bg_desc.entries = bg_entries;
    blit_sampler_bindgroup_ = device.CreateBindGroup(&bg_desc);

    // Create cache
    blit_bindgroup_cache_.create(device_, blit_render_pipeline_.GetBindGroupLayout(1));
}

void Renderer::destroy() {
    renderer2d_.destroy();
}

void Renderer::start_frame() {
    surface_view_ = device_.get_surface_texture_view();

    current_encoder_ = device_.device().CreateCommandEncoder();
    renderer2d_.start_frame(current_encoder_);

    blit_bindgroup_cache_.garbage_collect();
    blit_bindgroup_cache_.start_frame();
}

void Renderer::end_frame() {
    wgpu::CommandBuffer commands = current_encoder_.Finish();
    device_.queue().Submit(1, &commands);

    renderer2d_.end_frame();
    surface_view_ = nullptr;
    current_encoder_ = nullptr;
}

void Renderer::flush_2d(std::span<RenderView*> views) {
    renderer2d_.flush(views);
}

void Renderer::blit_to_surface(core::Handle<Texture> handle) {
    wgpu::RenderPassColorAttachment attachment{
        .view = surface_view_,
        .loadOp = wgpu::LoadOp::Clear,
        .storeOp = wgpu::StoreOp::Store,
        .clearValue = {0.0, 0.0, 0.0, 1.0},
    };

    wgpu::RenderPassDescriptor renderpass{
        .colorAttachmentCount = 1,
        .colorAttachments = &attachment,
    };

    BindGroupCacheKey<1> key{};
    key.textures[0] = handle;

    wgpu::RenderPassEncoder pass = current_encoder_.BeginRenderPass(&renderpass);
    pass.SetPipeline(blit_render_pipeline_);
    pass.SetBindGroup(0, blit_sampler_bindgroup_);
    pass.SetBindGroup(1, blit_bindgroup_cache_.get(key));
    pass.Draw(6);
    pass.End();
}

}  // namespace bvr::graphics
