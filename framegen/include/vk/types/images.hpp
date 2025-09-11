#pragma once

#include "vk/core/image.hpp"

#include <vulkan/vulkan_core.h>

#include <cstddef>
#include <vector>

namespace VK::Types {

    ///
    /// Fixed-size list of images allocated at runtime.
    ///
    template<size_t N>
    class Images {
    public:
        ///
        /// Create a new list of images.
        ///
        Images() : count(N) {
            images.reserve(N);
        }

        ///
        /// Move existing list of images.
        ///
        Images(const std::vector<Core::Image>& imgs) : images(imgs), count(images.size()) {
            if (count != N)
                throw std::invalid_argument("Invalid number of images");
        }

        ///
        /// Fill the list with images of similar properties.
        ///
        /// @param device The device to create the images on.
        /// @param extent The extent of each image.
        /// @param format The format of each image.
        /// @param usage The usage flags for each image.
        ///
        /// @throws VK::vulkan_error if object creation fails.
        ///
        void fill(const Core::Device& device, VkExtent2D extent,
                VkFormat format = VK_FORMAT_R8G8B8A8_UNORM,
                VkImageUsageFlags usage =
                    VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT) {
            for (size_t i = 0; i < count; ++i)
                images.emplace_back(device, extent, format, usage);
        }

        ///
        /// Emplace a new image into the list.
        ///
        /// @param img The image to emplace.
        ///
        /// @throws std::length_error if the list is already full.
        ///
        void emplace(const Core::Image& img) {
            if (images.size() >= count)
                throw std::length_error("Image list is full");
            images.emplace_back(img);
        }

        ///
        /// Get the image at the specified index.
        ///
        /// @param index The index of the image to get.
        /// @return The image at the specified index.
        ///
        /// @throws std::out_of_range if the index is out of range.
        ///
        [[nodiscard]] const Core::Image& at(size_t index) const {
            if (index >= count) {
                throw std::out_of_range("Index out of range");
            }
            return images.at(index);
        }

        ///
        /// Get all images.
        ///
        /// @return A const reference to the vector of images.
        ///
        [[nodiscard]] const std::vector<Core::Image>& all() const {
            return images;
        }

        // add other members as needed

    private:
        std::vector<Core::Image> images;
        size_t count{0};
    };

}
