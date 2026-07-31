#include "beaver/graphics/renderer2d.h"

#include "beaver/app/applicationcontext.h"
#include "beaver/graphics/bindgroupcache.h"
#include "beaver/graphics/gpuresources.h"
#include "beaver/graphics/renderview.h"
#include "beaver/graphics/shaders/render2d.wgsl.h"
#include "webgpu/webgpu_cpp.h"

namespace bvr::gfx {

void Renderer2D::create(app::ApplicationContext& context) {
    context_ = &context;

    // Init the pipeline
    wgpu::ShaderSourceWGSL wgsl{{.code = render2d_wgsl}};

    wgpu::ShaderModuleDescriptor shaderModuleDescriptor{.nextInChain = &wgsl};
    wgpu::ShaderModule shaderModule = context_->device->device().CreateShaderModule(&shaderModuleDescriptor);

    std::vector<wgpu::VertexAttribute> attributes(4);
    attributes[0].format = wgpu::VertexFormat::Uint32;
    attributes[0].offset = offsetof(RenderCommand2D, texture_id);
    attributes[0].shaderLocation = 0;

    attributes[1].format = wgpu::VertexFormat::Uint16x4;
    attributes[1].offset = offsetof(RenderCommand2D, position);
    attributes[1].shaderLocation = 1;

    attributes[2].format = wgpu::VertexFormat::Float32x4;
    attributes[2].offset = offsetof(RenderCommand2D, uv);
    attributes[2].shaderLocation = 2;

    attributes[3].format = wgpu::VertexFormat::Unorm8x4;
    attributes[3].offset = offsetof(RenderCommand2D, colour);
    attributes[3].shaderLocation = 3;

    wgpu::VertexBufferLayout vb_layout = {
        .stepMode = wgpu::VertexStepMode::Instance,
        .arrayStride = sizeof(RenderCommand2D),
        .attributeCount = attributes.size(),
        .attributes = attributes.data(),
    };

    wgpu::BlendState blend_state;
    blend_state.color.operation = wgpu::BlendOperation::Add;
    blend_state.color.srcFactor = wgpu::BlendFactor::SrcAlpha;
    blend_state.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
    blend_state.alpha.operation = wgpu::BlendOperation::Add;
    blend_state.alpha.srcFactor = wgpu::BlendFactor::One;
    blend_state.alpha.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;

    wgpu::ColorTargetState colorTargetState{
        .format = wgpu::TextureFormat::RGBA8Unorm,
        .blend = &blend_state,
        .writeMask = wgpu::ColorWriteMask::All,
    };

    wgpu::FragmentState fragmentState{
        .module = shaderModule,
        .targetCount = 1,
        .targets = &colorTargetState,
    };

    wgpu::RenderPipelineDescriptor descriptor{
        .vertex = {
            .module = shaderModule,
            .bufferCount = 1,
            .buffers = &vb_layout,
        },
        .fragment = &fragmentState,
    };
    render_pipeline_ = context_->device->device().CreateRenderPipeline(&descriptor);

    // Create the Command Buffer with a default size
    BufferDescriptor buffer_desc{
        .label = "Command Buffer",
        .size = 1024 * sizeof(RenderCommand2D),
        .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex,
    };
    command_buffer_ = context.device->create_buffer(buffer_desc);

    // Create the uniform buffer
    buffer_desc = {
        .label = "Uniform Buffer",
        .size = sizeof(RenderUniforms),
        .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform,
    };
    uniforms_buffer_ = context.device->create_buffer(buffer_desc);

    // Create the sampler
    wgpu::SamplerDescriptor samplerDesc;
    samplerDesc.magFilter = wgpu::FilterMode::Nearest;
    samplerDesc.minFilter = wgpu::FilterMode::Nearest;
    samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Nearest;
    samplerDesc.addressModeU = wgpu::AddressMode::ClampToEdge;
    samplerDesc.addressModeV = wgpu::AddressMode::ClampToEdge;
    samplerDesc.addressModeW = wgpu::AddressMode::ClampToEdge;
    sampler_ = context.device->device().CreateSampler(&samplerDesc);

    // Create the bindgroup
    std::vector<wgpu::BindGroupEntry> bg_entries(2);
    bg_entries[0].binding = 0;
    bg_entries[0].buffer = context.device->get_buffer(uniforms_buffer_).buffer;

    bg_entries[1].binding = 1;
    bg_entries[1].sampler = sampler_;

    wgpu::BindGroupDescriptor bg_desc{};
    bg_desc.layout = render_pipeline_.GetBindGroupLayout(0);
    bg_desc.entryCount = static_cast<uint32_t>(bg_entries.size());
    bg_desc.entries = bg_entries.data();

    render_bindgroup_ = context_->device->device().CreateBindGroup(&bg_desc);

    // Create the texture bindgroup cache
    render_bindgroup_cache_.create(context, render_pipeline_.GetBindGroupLayout(1));
}

void Renderer2D::destroy() {}

void Renderer2D::start_frame(wgpu::CommandEncoder& encoder) {
    current_command_encoder_ = encoder;

    render_bindgroup_cache_.garbage_collect();
    render_bindgroup_cache_.start_frame();
}

void Renderer2D::end_frame() {
    commands_.clear();
    batches_.clear();
    batch_count_ = 0;

    current_command_encoder_ = nullptr;
}

void Renderer2D::draw_textured_rect(core::Handle<Texture> handle, core::Rect<uint16_t> position, core::Rect<float> uv, core::Colour<uint8_t> colour) {
    // Is the handle in the current batch?
    int index = -1;
    for (int i = 0; i < texture_count_; i++) {
        if (active_textures_.textures[i] == handle) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        // Havent seen this texture yet
        if (texture_count_ >= MAX_TEXTURE_SLOTS) {
            // Break the batch
            batches_.emplace_back(RenderBatch2D{
                active_textures_,
                batch_count_,
            });

            active_textures_.textures[1] = white_pixel_;
            active_textures_.textures[2] = white_pixel_;
            active_textures_.textures[3] = white_pixel_;

            batch_count_ = 0;
            texture_count_ = 0;
        }

        index = texture_count_;
        active_textures_.textures[texture_count_] = handle;
        texture_count_++;
    }

    // Generate the render command using the batch texture position
    commands_.emplace_back(RenderCommand2D{
        .texture_id = static_cast<uint32_t>(index),
        .position = position,
        .uv = uv,
        .colour = colour,
    });

    batch_count_++;
}

void Renderer2D::flush(RenderView& view, bool clear_background) {
    // Push the current render command buffer to the GPU
    // We must ensure the size is sufficient, otherwise we'll need to recreate the buffer

    uint64_t commands_byte_size = commands_.size() * sizeof(RenderCommand2D);
    Buffer& draw_buffer = context_->device->get_buffer(command_buffer_);
    if (draw_buffer.size <= commands_byte_size) {
        // TODO(jvsluis): Need to resize
    }

    context_->device->write_buffer(command_buffer_, commands_.data(), 0, commands_byte_size);

    // Update the uniforms
    context_->device->write_buffer(uniforms_buffer_, &view.uniforms, 0, sizeof(RenderUniforms));

    wgpu::TextureView texture_view = context_->device->get_texture(view.colour_target).view;

    wgpu::RenderPassColorAttachment attachment{
        .view = texture_view,
        .loadOp = clear_background ? wgpu::LoadOp::Clear : wgpu::LoadOp::Load,
        .storeOp = wgpu::StoreOp::Store,
        .clearValue = {0.0, 0.0, 0.0, 1.0},
    };

    wgpu::RenderPassDescriptor renderpass{.colorAttachmentCount = 1,
                                          .colorAttachments = &attachment};

    wgpu::RenderPassEncoder pass = current_command_encoder_.BeginRenderPass(&renderpass);

    pass.SetViewport(
        view.viewport.x, view.viewport.y,
        view.viewport.width, view.viewport.height,
        0.0, 1.0);

    if (batch_count_ > 0) {
        batches_.emplace_back(RenderBatch2D{
            active_textures_,
            batch_count_,
        });
    }
    batch_count_ = 0;

    uint64_t offset = 0;
    for (auto& batch : batches_) {
        uint64_t batch_width = sizeof(RenderCommand2D) * batch.size;

        pass.SetPipeline(render_pipeline_);
        pass.SetBindGroup(0, render_bindgroup_);
        pass.SetBindGroup(1, render_bindgroup_cache_.get(batch.key));
        pass.SetVertexBuffer(0, context_->device->get_buffer(command_buffer_).buffer, offset, batch_width);
        pass.Draw(6, commands_.size());

        offset += batch.size * batch_width;
    }

    pass.End();
}

}  // namespace bvr::gfx
