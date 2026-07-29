#pragma once

#include <memory>

#include "beaver/app/window.h"
#include "beaver/graphics/device.h"

namespace bvr::app {

struct ApplicationContext {
    std::unique_ptr<Window> window;
    std::unique_ptr<gfx::Device> device;
};

}  // namespace bvr::app
