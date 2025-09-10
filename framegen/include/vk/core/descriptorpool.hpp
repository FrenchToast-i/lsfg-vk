#pragma once

#include "vk/core/device.hpp"

#include <vulkan/vulkan_core.h>

#include <memory>

namespace VK::Core {

    ///
    /// C++ wrapper class for a Vulkan descriptor pool.
    ///
    /// This class manages the lifetime of a Vulkan descriptor pool.
    ///
    class DescriptorPool {
    public:
        ///
        /// Create the descriptor pool.
        ///
        /// @param device Vulkan device
        ///
        /// @throws VK::vulkan_error if object creation fails.
        ///
        DescriptorPool(const Device& device);

        /// Get the Vulkan handle.
        [[nodiscard]] auto handle() const { return *this->descriptorPool; }
    private:
        std::shared_ptr<VkDescriptorPool> descriptorPool;
    };

}
