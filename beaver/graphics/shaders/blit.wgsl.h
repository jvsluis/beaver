#pragma once

const char blit_wgsl[] = R"(
struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) uv: vec2f,
};

@group(0) @binding(0) var blit_sampler: sampler;
@group(1) @binding(0) var blit_texture: texture_2d<f32>;

@vertex fn vertex_main(@builtin(vertex_index) i : u32) -> VertexOutput {
    var pos = array<vec2f, 6>(
        vec2f(-1.0, -1.0), vec2f( 1.0, -1.0), vec2f(-1.0,  1.0),
        vec2f(-1.0,  1.0), vec2f( 1.0, -1.0), vec2f( 1.0,  1.0)
    );

    // Corresponding UV coordinates
    // Note: Y is flipped in WebGPU UV space vs NDC
    var uv = array<vec2f, 6>(
        vec2f(0.0, 1.0), vec2f(1.0, 1.0), vec2f(0.0, 0.0),
        vec2f(0.0, 0.0), vec2f(1.0, 1.0), vec2f(1.0, 0.0)
    );

    var out: VertexOutput;
    out.position = vec4f(pos[i], 0.0, 1.0);
    out.uv = uv[i];
    return out;
}

@fragment fn fragment_main(@location(0) uv: vec2f) -> @location(0) vec4f {
    return textureSample(blit_texture, blit_sampler, uv);
}
)";
