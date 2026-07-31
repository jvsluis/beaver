#pragma once

#include <memory>

#include "beaver/app/window.h"
#include "beaver/assets/assetmanager.h"
#include "beaver/graphics/device.h"

namespace bvr::app {

struct ApplicationContext {
    std::unique_ptr<Window> window;
    std::unique_ptr<gfx::Device> device;
    std::unique_ptr<asset::AssetManager> asset_manager;
};

}  // namespace bvr::app
