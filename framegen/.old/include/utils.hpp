#pragma once

#include "core/commandbuffer.hpp"
#include "core/commandpool.hpp"
#include "core/descriptorpool.hpp"
#include "core/image.hpp"
#include "core/device.hpp"
#include "pool/resourcepool.hpp"
#include "pool/shaderpool.hpp"

#include <vulkan/vulkan_core.h>

#include <optional>
#include <cstddef>
#include <cstdint>
#include <string>
#include <array>
#include <vector>

namespace LSFG::Utils {

    ///
    /// Upload a DDS file to a Vulkan image.
    ///
    /// @param device The Vulkan device
    /// @param commandPool The command pool
    /// @param image The Vulkan image to upload to
    /// @param path The path to the DDS file.
    ///
    /// @throws std::system_error If the file cannot be opened or read.
    /// @throws ls:vulkan_error If the Vulkan image cannot be created or updated.
    ///
    void uploadImage(const Core::Device& device,
        const Core::CommandPool& commandPool,
        Core::Image& image, const std::string& path);

    ///
    /// Clear a texture to white during setup.
    ///
    /// @param device The Vulkan device.
    /// @param image The image to clear.
    /// @param white If true, the image will be cleared to white, otherwise to black.
    ///
    /// @throws LSFG::vulkan_error If the Vulkan image cannot be cleared.
    ///
    void clearImage(const Core::Device& device, Core::Image& image, bool white = false);

}

namespace LSFG {
    struct Vulkan {
        Core::Device device;
        Core::CommandPool commandPool;
        Core::DescriptorPool descriptorPool;

        uint64_t generationCount;
        float flowScale;
        bool isHdr;

        Pool::ShaderPool shaders;
        Pool::ResourcePool resources;
    };
}
