#include "beaver/graphics/renderer2d.h"

#include <cstring>

#include "beaver/graphics/bindgroupcache.h"
#include "beaver/graphics/gpuresources.h"
#include "beaver/graphics/renderview.h"
#include "beaver/graphics/shaders/render2d.wgsl.h"
#include "webgpu/webgpu_cpp.h"

namespace bvr::graphics {

void Renderer2D::create() {
    // Init the pipeline
    wgpu::ShaderSourceWGSL wgsl{{.code = render2d_wgsl}};

    wgpu::ShaderModuleDescriptor shaderModuleDescriptor{.nextInChain = &wgsl};
    wgpu::ShaderModule shaderModule = device_.device().CreateShaderModule(&shaderModuleDescriptor);

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

    // Define bindgroup layouts
    // Uniform
    wgpu::BindGroupLayoutEntry bg0_entry = {};
    bg0_entry.binding = 0;
    bg0_entry.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
    bg0_entry.buffer.type = wgpu::BufferBindingType::Uniform;
    bg0_entry.buffer.hasDynamicOffset = true;
    bg0_entry.buffer.minBindingSize = sizeof(RenderUniforms);

    wgpu::BindGroupLayoutDescriptor bg0_desc = {};
    bg0_desc.label = "Group 0: Uniforms";
    bg0_desc.entryCount = 1;
    bg0_desc.entries = &bg0_entry;

    wgpu::BindGroupLayout layoutGroup0 = device_.device().CreateBindGroupLayout(&bg0_desc);

    // Sampler
    wgpu::BindGroupLayoutEntry bg1_entry = {};
    bg1_entry.binding = 0;
    bg1_entry.visibility = wgpu::ShaderStage::Fragment;
    bg1_entry.sampler.type = wgpu::SamplerBindingType::Filtering;

    wgpu::BindGroupLayoutDescriptor bg1_desc = {};
    bg1_desc.label = "Group 1: Sampler";
    bg1_desc.entryCount = 1;
    bg1_desc.entries = &bg1_entry;

    wgpu::BindGroupLayout layoutGroup1 = device_.device().CreateBindGroupLayout(&bg1_desc);

    // Textures
    std::vector<wgpu::BindGroupLayoutEntry> bg2_entries(4);

    for (uint32_t i = 0; i < 4; ++i) {
        bg2_entries[i].binding = i;
        bg2_entries[i].visibility = wgpu::ShaderStage::Fragment;
        bg2_entries[i].texture.sampleType = wgpu::TextureSampleType::Float;
        bg2_entries[i].texture.viewDimension = wgpu::TextureViewDimension::e2D;
        bg2_entries[i].texture.multisampled = false;
    }

    wgpu::BindGroupLayoutDescriptor bg2_desc = {};
    bg2_desc.label = "Group 2: Textures";
    bg2_desc.entryCount = static_cast<uint32_t>(bg2_entries.size());
    bg2_desc.entries = bg2_entries.data();

    wgpu::BindGroupLayout layoutGroup2 = device_.device().CreateBindGroupLayout(&bg2_desc);

    // Create Pipeline Layout
    wgpu::BindGroupLayout bindGroupLayouts[] = {layoutGroup0, layoutGroup1, layoutGroup2};

    wgpu::PipelineLayoutDescriptor pipelineLayoutDesc = {};
    pipelineLayoutDesc.label = "Main Pipeline Layout";
    pipelineLayoutDesc.bindGroupLayoutCount = 3;
    pipelineLayoutDesc.bindGroupLayouts = bindGroupLayouts;

    wgpu::PipelineLayout explicitPipelineLayout = device_.device().CreatePipelineLayout(&pipelineLayoutDesc);

    // Create the Pipeline
    wgpu::RenderPipelineDescriptor descriptor{
        .label = "Main Render Pipeline",
        .layout = explicitPipelineLayout,
        .vertex = {
            .module = shaderModule,
            .bufferCount = 1,
            .buffers = &vb_layout,
        },
        .fragment = &fragmentState,
    };
    render_pipeline_ = device_.device().CreateRenderPipeline(&descriptor);

    // Create the Command Buffer with a default size
    BufferDescriptor buffer_desc{
        .label = "Command Buffer",
        .size = 1024 * sizeof(RenderCommand2D),
        .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex,
    };
    command_buffer_ = device_.create_buffer(buffer_desc);

    // Create the uniform buffer
    buffer_desc = {
        .label = "Uniform Buffer",
        .size = 1024,  // 4 * sizeof(RenderUniforms),
        .usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform,
    };
    uniforms_buffer_ = device_.create_buffer(buffer_desc);

    // Create the sampler
    wgpu::SamplerDescriptor samplerDesc;
    samplerDesc.magFilter = wgpu::FilterMode::Nearest;
    samplerDesc.minFilter = wgpu::FilterMode::Nearest;
    samplerDesc.mipmapFilter = wgpu::MipmapFilterMode::Nearest;
    samplerDesc.addressModeU = wgpu::AddressMode::ClampToEdge;
    samplerDesc.addressModeV = wgpu::AddressMode::ClampToEdge;
    samplerDesc.addressModeW = wgpu::AddressMode::ClampToEdge;
    sampler_ = device_.device().CreateSampler(&samplerDesc);

    // Create the bindgroup
    std::vector<wgpu::BindGroupEntry> bg_entries0(1);
    bg_entries0[0].binding = 0;
    bg_entries0[0].buffer = device_.get_buffer(uniforms_buffer_).buffer;
    bg_entries0[0].offset = 0;
    bg_entries0[0].size = sizeof(RenderUniforms);

    wgpu::BindGroupDescriptor bg_desc0{};
    bg_desc0.layout = layoutGroup0;
    bg_desc0.entryCount = static_cast<uint32_t>(bg_entries0.size());
    bg_desc0.entries = bg_entries0.data();

    uniform_bindgroup_ = device_.device().CreateBindGroup(&bg_desc0);

    std::vector<wgpu::BindGroupEntry> bg_entries1(1);
    bg_entries1[0].binding = 0;
    bg_entries1[0].sampler = sampler_;

    wgpu::BindGroupDescriptor bg_desc1{};
    bg_desc1.layout = layoutGroup1;
    bg_desc1.entryCount = static_cast<uint32_t>(bg_entries1.size());
    bg_desc1.entries = bg_entries1.data();

    sampler_bindgroup_ = device_.device().CreateBindGroup(&bg_desc1);

    // Create the texture bindgroup cache
    render_bindgroup_cache_.create(device_, layoutGroup2);
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

void Renderer2D::flush(std::span<RenderView*> views) {
    // Push the current render command buffer to the GPU
    // We must ensure the size is sufficient, otherwise we'll need to recreate the buffer

    uint64_t commands_byte_size = commands_.size() * sizeof(RenderCommand2D);
    Buffer& draw_buffer = device_.get_buffer(command_buffer_);
    if (draw_buffer.size <= commands_byte_size) {
        // TODO(jvsluis): Need to resize
    }

    device_.write_buffer(command_buffer_, commands_.data(), 0, commands_byte_size);

    // Update the uniforms using dynamic offsets for each view
    const size_t uniform_stride = 256;

    std::vector<uint8_t> uniform_upload_data(4 * uniform_stride, 0);
    for (int i = 0; i < views.size(); i++) {
        std::memcpy(uniform_upload_data.data() + (i * uniform_stride), &views[i]->uniforms, sizeof(RenderUniforms));
    }

    device_.write_buffer(uniforms_buffer_, uniform_upload_data.data(), 0, 4 * uniform_stride);

    // Do a full flush once per view, using dynamic offset uniforms for each view
    for (int i = 0; i < views.size(); i++) {
        RenderView* view = views[i];

        wgpu::TextureView texture_view = device_.get_texture(view->colour_target).view;

        wgpu::RenderPassColorAttachment attachment{
            .view = texture_view,
            .loadOp = view->clearColourTarget ? wgpu::LoadOp::Clear : wgpu::LoadOp::Load,
            .storeOp = wgpu::StoreOp::Store,
            .clearValue = {0.0, 0.0, 0.0, 1.0},
        };

        wgpu::RenderPassDescriptor renderpass{.colorAttachmentCount = 1,
                                              .colorAttachments = &attachment};

        wgpu::RenderPassEncoder pass = current_command_encoder_.BeginRenderPass(&renderpass);

        pass.SetViewport(
            view->viewport.x, view->viewport.y,
            view->viewport.width, view->viewport.height,
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

            uint32_t dynamicOffset = static_cast<uint32_t>(i * uniform_stride);
            pass.SetBindGroup(0, uniform_bindgroup_, 1, &dynamicOffset);
            pass.SetBindGroup(1, sampler_bindgroup_);
            pass.SetBindGroup(2, render_bindgroup_cache_.get(batch.key));
            pass.SetVertexBuffer(0, device_.get_buffer(command_buffer_).buffer, offset, batch_width);
            pass.Draw(6, commands_.size());

            offset += batch.size * batch_width;
        }

        pass.End();
    }
}

}  // namespace bvr::graphics
