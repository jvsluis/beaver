#pragma once

#include "beaver/core/geometry.h"
#include "beaver/core/handle.h"
#include "beaver/graphics/gpuresources.h"

namespace bvr::gfx {

struct RenderView {
    core::Handle<Texture> colour_target;
    core::Rect<uint32_t> viewport;

    // TODO(jvsluis): View Matrix / proj matrix goes here
};

}  // namespace bvr::gfx
