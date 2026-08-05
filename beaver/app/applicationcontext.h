#pragma once

#include "beaver/app/window.h"
#include "beaver/assets/assetmanager.h"
#include "beaver/core/taskqueue.h"
#include "beaver/graphics/device.h"

namespace bvr::app {

struct EngineContext {
    Window& window;
    gfx::Device& device;
    asset::AssetManager& asset_manager;
    core::TaskQueue& main_thread_queue;
};

}  // namespace bvr::app
