#pragma once

#include "vk/core/device.hpp"
#include "vk/core/image.hpp"

#include <vulkan/vulkan_core.h>

#include <algorithm>
#include <cstddef>
#include <vector>

namespace VK::Helper {

    ///
    /// Helper class for a list of images with similar properties.
    ///
    class ImageGroup {
    public:
        ImageGroup() noexcept = default;

        ///
        /// Create an image group.
        ///
        /// @param device Vulkan device.
        /// @param extent Extent of the base level.
        /// @param count Number of images (must be at least 1).
        /// @param format Image format.
        ///
        /// @throws VK::vulkan_error if object creation fails.
        ///
        ImageGroup(const Core::Device& device, VkExtent2D extent,
            size_t count, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM);

        /// Get the number of images
        [[nodiscard]] auto count() const noexcept { return this->images.size(); }
        /// Get a specific image.
        [[nodiscard]] const auto& get(size_t i) const { return this->images.at(i); }
        /// Get all images.
        [[nodiscard]] const auto& into() const noexcept { return this->images; }

        /// Get a subgroup of images
        [[nodiscard]] std::vector<Core::Image> subGroup(size_t start, size_t length) const {
            if (start + length > this->count())
                throw std::out_of_range("Sub group range out of bounds");

            std::vector<Core::Image> subgroup(length);
            std::copy_n(&this->images.at(start), length, subgroup.data());
            return subgroup;
        }
    private:
        std::vector<Core::Image> images;
    };

}
