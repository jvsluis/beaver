#include "beaver/assets/assetmanager.h"

#include <webgpu/webgpu_cpp.h>

#include <fstream>

#include "beaver/core/log.h"
#include "beaver/graphics/device.h"
#include "beaver/graphics/gpuresources.h"

#define STB_IMAGE_IMPLEMENTATION
#include "beaver/thirdparty/stb_image/stb_image.h"

namespace bvr::assets {

void AssetManager::create(graphics::Device& device) {
    device_ = &device;
}

// TODO(jvsluis): make this web friendly
core::Handle<graphics::Texture> AssetManager::load_texture(const std::string& file_path) {
    auto it = texture_cache_.find(file_path);
    if (it != texture_cache_.end()) {
        return it->second;
    }

    std::ifstream file(file_path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        CORE_ERROR("Failed to load texture file (%s). Defaulting to magenta pixel", file_path.c_str());
        return device_->magenta_pixel();
    }
    // Read from Disk
    int size = file.tellg();
    file.seekg(0, std::ios::beg);

    uint8_t* buffer = (uint8_t*)malloc(size);
    file.read((char*)buffer, size);

    std::vector<uint8_t> vec_buffer(buffer, buffer + size);

    const int desired_channels = 4;
    int original_channels = 0;
    int width, height = 0;

    uint8_t* decoded_pixels = stbi_load_from_memory(
        vec_buffer.data(),
        static_cast<int>(vec_buffer.size()),
        &width,
        &height,
        &original_channels,
        desired_channels);

    uint32_t uwidth = static_cast<uint32_t>(width);
    uint32_t uheight = static_cast<uint32_t>(height);

    if (!decoded_pixels) {
        CORE_FATAL("Failure loading encoded image: %s", stbi_failure_reason());
        return device_->magenta_pixel();
    }

    // Calculate the size of the uncompressed data
    std::size_t total_bytes = static_cast<std::size_t>(uwidth * uheight * desired_channels);

    graphics::TextureDescriptor desc = {
        .label = "",
        .width = uwidth,
        .height = uheight,
        .usage = wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::CopyDst,
        .format = wgpu::TextureFormat::RGBA8Unorm,
    };

    auto handle = device_->create_texture(desc);
    device_->write_texture(handle, decoded_pixels, total_bytes);

    free(buffer);
    stbi_image_free(decoded_pixels);

    texture_cache_[file_path] = handle;
    return handle;
}

}  // namespace bvr::assets
