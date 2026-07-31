#pragma once

const char render2d_wgsl[] = R"(
    @vertex fn vertexMain(@builtin(vertex_index) i : u32) ->
     @builtin(position) vec4f {
       const pos = array(vec2f(0.0, 1.0), vec2f(-1.0, -1.0), vec2f(1.0, -1.0));
       return vec4f(pos[i], 0.0, 1.0);
   }
   @fragment fn fragmentMain() -> @location(0) vec4f {
       return vec4f(0.0, 1.0, 0.0, 1.0);
   }

)";
