#pragma once

#include <unordered_map>

#include "beaver/core/handle.h"
#include "beaver/graphics/device.h"
#include "beaver/graphics/gpuresources.h"

namespace bvr::assets {

class AssetManager {
public:
    void create(graphics::Device& device);

    core::Handle<graphics::Texture> load_texture(const std::string& file_path);

private:
    graphics::Device* device_;
    std::unordered_map<std::string, core::Handle<graphics::Texture>> texture_cache_;
};

}  // namespace bvr::assets
