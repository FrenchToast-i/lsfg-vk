#pragma once

#include "vk/core/device.hpp"

#include <optional>
#include <vulkan/vulkan_core.h>

#include <memory>

namespace VK::Core {

    ///
    /// C++ wrapper class for a Vulkan image.
    ///
    /// This class manages the lifetime of a Vulkan image.
    ///
    class Image {
    public:
        Image() noexcept = default;

        ///
        /// Create the image.
        ///
        /// @param device Vulkan device
        /// @param extent Extent of the image in pixels.
        /// @param format Vulkan format of the image
        /// @param usage Usage flags for the image
        /// @param importFd Optional file descriptor for shared memory.
        /// @param exportFd Optional pointer to an integer where the file descriptor will be stored.
        ///
        /// @throws VK::vulkan_error if object creation fails.
        ///
        Image(const Device& device, VkExtent2D extent,
            VkFormat format = VK_FORMAT_R8G8B8A8_UNORM,
            VkImageUsageFlags usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            std::optional<int> importFd = std::nullopt,
            std::optional<int*> exportFd = std::nullopt);

        /// Get the Vulkan handle.
        [[nodiscard]] auto handle() const { return *this->image; }
        /// Get the Vulkan device memory handle.
        [[nodiscard]] auto getMemory() const { return *this->memory; }
        /// Get the Vulkan image view handle.
        [[nodiscard]] auto getView() const { return *this->view; }
        /// Get the extent of the image.
        [[nodiscard]] auto getExtent() const { return this->extent; }
        /// Get the format of the image.
        [[nodiscard]] auto getFormat() const { return this->format; }
    private:
        std::shared_ptr<VkImage> image;
        std::shared_ptr<VkDeviceMemory> memory;
        std::shared_ptr<VkImageView> view;

        VkExtent2D extent{};
        VkFormat format{};
    };

}
