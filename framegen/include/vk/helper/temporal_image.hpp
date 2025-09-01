#pragma once

#include "vk/core/device.hpp"
#include "vk/core/image.hpp"

#include <vulkan/vulkan_core.h>

#include <cstddef>
#include <vector>

namespace VK::Helper {

    ///
    /// Helper class for a list of temporal images.
    ///
    class TemporalImage {
    public:
        TemporalImage() noexcept = default;

        ///
        /// Create a temporal image.
        ///
        /// @param device Vulkan device.
        /// @param extent Extent of the images.
        /// @param count Amount of images (must be at least 1).
        /// @param format Image format.
        ///
        /// @throws VK::vulkan_error if object creation fails.
        ///
        TemporalImage(const Core::Device& device, VkExtent2D extent,
            size_t count, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM);

        ///
        /// Create a temporal image based on existing images.
        ///
        /// @param images Vector of existing images.
        ///
        TemporalImage(std::vector<Core::Image> images)
            : images(std::move(images)) {}

        /// Get the number of images.
        [[nodiscard]] auto count() const noexcept { return this->images.size(); }
        /// Get a specific image.
        [[nodiscard]] const auto& get(size_t i) const { return this->images.at(i); }
        /// Get the image based on frame index.
        [[nodiscard]] const auto& at(size_t f) const { return this->images.at(f % this->count()); }
        /// Get all images.
        [[nodiscard]] const auto& into() const noexcept { return this->images; }
    private:
        std::vector<Core::Image> images;
    };

}
