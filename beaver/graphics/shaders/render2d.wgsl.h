#pragma once

const char render2d_wgsl[] = R"(
struct InstanceInput {
    @location(0) texture_id: u32,
    @location(1) pos_rect: vec4<u32>,
    @location(2) uv_rect: vec4<f32>,
    @location(3) colour: vec4<f32>,
};

struct FragmentInput {
    @builtin(position) position: vec4<f32>,
    @location(0) @interpolate(flat) texture_id: u32,
    @location(1) uv: vec2<f32>,
    @location(2) colour: vec4<f32>,
}

struct Uniforms {
    projection_mtx: mat4x4<f32>,
    view_mtx: mat4x4<f32>,
};

const unit_quad = array<vec2<f32>, 6>(
    vec2(0.0, 0.0), // Top Left
    vec2(1.0, 0.0), // Top Right
    vec2(0.0, 1.0), // Bottom Left
    vec2(0.0, 1.0), // Bottom Left (Repeated for 2nd triangle)
    vec2(1.0, 0.0), // Top Right (Repeated for 2nd triangle)
    vec2(1.0, 1.0)  // Bottom Right
);

@group(0) @binding(0) var<uniform> uniforms: Uniforms;
@group(1) @binding(0) var tex_sampler: sampler;
@group(2) @binding(0) var tex_0: texture_2d<f32>;
@group(2) @binding(1) var tex_1: texture_2d<f32>;
@group(2) @binding(2) var tex_2: texture_2d<f32>;
@group(2) @binding(3) var tex_3: texture_2d<f32>;

@vertex fn vertexMain(@builtin(vertex_index) v_idx: u32, @builtin(instance_index) i_idx: u32, input: InstanceInput) -> FragmentInput {
    let quad = unit_quad[v_idx];
    let pos = vec2<f32>(
    f32(input.pos_rect.x) + (quad.x * f32(input.pos_rect.z)),
    f32(input.pos_rect.y) + (quad.y * f32(input.pos_rect.w))
    );

    let uv = vec2<f32>(
        input.uv_rect.x + (quad.x * input.uv_rect.z),
        input.uv_rect.y + (quad.y * input.uv_rect.w)
    );

    var output : FragmentInput;
    output.texture_id = input.texture_id;
    output.position = uniforms.projection_mtx * uniforms.view_mtx * vec4<f32>(vec2f(pos), 0.0, 1.0);
    output.uv = uv;
    output.colour = input.colour;
    return output;
}

@fragment fn fragmentMain(input: FragmentInput) -> @location(0) vec4f {
    var colour: vec4<f32>;

    switch input.texture_id {
        case 0u: { colour = textureSampleLevel(tex_0, tex_sampler, input.uv, 0.0); }
        case 1u: { colour = textureSampleLevel(tex_1, tex_sampler, input.uv, 0.0); }
        case 2u: { colour = textureSampleLevel(tex_2, tex_sampler, input.uv, 0.0); }
        case 3u: { colour = textureSampleLevel(tex_3, tex_sampler, input.uv, 0.0); }
        default: { colour = vec4<f32>(1.0, 0.0, 1.0, 1.0); } // Magenta error fallback
    }

    return colour * input.colour;
}
)";
