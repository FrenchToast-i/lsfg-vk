#pragma once

#include "vk/core/device.hpp"

#include <vulkan/vulkan_core.h>

#include <memory>

namespace VK::Core {

    ///
    /// C++ wrapper class for a Vulkan command pool.
    ///
    /// This class manages the lifetime of a Vulkan command pool.
    ///
    class CommandPool {
    public:
        CommandPool() noexcept = default;

        ///
        /// Create the command pool.
        ///
        /// @param device Vulkan device
        ///
        /// @throws VK::vulkan_error if object creation fails.
        ///
        CommandPool(const Device& device);

        /// Get the Vulkan handle.
        [[nodiscard]] auto handle() const { return *this->commandPool; }

        /// Trivially copyable, moveable and destructible
        CommandPool(const CommandPool&) noexcept = default;
        CommandPool& operator=(const CommandPool&) noexcept = default;
        CommandPool(CommandPool&&) noexcept = default;
        CommandPool& operator=(CommandPool&&) noexcept = default;
        ~CommandPool() = default;
    private:
        std::shared_ptr<VkCommandPool> commandPool;
    };

}
