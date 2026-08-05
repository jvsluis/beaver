#pragma once

#include "beaver/core/geometry.h"
#include "beaver/core/handle.h"
#include "beaver/graphics/gpuresources.h"
#include "glm/ext/matrix_float4x4.hpp"

namespace bvr::graphics {

struct RenderUniforms {
    glm::mat4x4 projection_matrix;
    glm::mat4x4 view_matrix;
};

struct RenderView {
    core::Handle<Texture> colour_target;
    core::Rect<uint32_t> viewport;
    bool clearColourTarget = true;

    RenderUniforms uniforms;
};

}  // namespace bvr::graphics
