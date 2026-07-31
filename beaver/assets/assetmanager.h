#pragma once

#include <unordered_map>

#include "beaver/core/handle.h"
#include "beaver/graphics/device.h"
#include "beaver/graphics/gpuresources.h"

namespace bvr::asset {

class AssetManager {
public:
    void create(gfx::Device& device);

    core::Handle<gfx::Texture> load_texture(const std::string& file_path);

private:
    gfx::Device* device_;
    std::unordered_map<std::string, core::Handle<gfx::Texture>> texture_cache_;
};

}  // namespace bvr::asset
