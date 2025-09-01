#pragma once

#include "vk/core/device.hpp"
#include "vk/core/image.hpp"

#include <vulkan/vulkan_core.h>

#include <cstddef>
#include <vector>

namespace VK::Helper {

    ///
    /// Helper class for a list of mipmapped images.
    ///
    class MipmappedImage {
    public:
        MipmappedImage() noexcept = default;

        ///
        /// Create a mipmappped image.
        ///
        /// @param device Vulkan device.
        /// @param extent Extent of the base level.
        /// @param levels Number of mip levels (must be at least 1).
        /// @param format Image format.
        ///
        /// @throws VK::vulkan_error if object creation fails.
        ///
        MipmappedImage(const Core::Device& device, VkExtent2D extent,
            size_t levels, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM);

        /// Get the number of mip levels.
        [[nodiscard]] auto levels() const noexcept { return this->images.size(); }
        /// Get a specific mip level image.
        [[nodiscard]] const auto& get(size_t i) const { return this->images.at(i); }
        /// Get all mip level images.
        [[nodiscard]] const auto& into() const noexcept { return this->images; }
    private:
        std::vector<Core::Image> images;
    };

}
