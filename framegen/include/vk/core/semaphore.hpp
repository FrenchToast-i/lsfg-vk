#pragma once

#include "vk/core/device.hpp"

#include <vulkan/vulkan_core.h>

#include <optional>
#include <memory>

namespace VK::Core {

    ///
    /// C++ wrapper class for a Vulkan semaphore.
    ///
    /// This class manages the lifetime of a Vulkan semaphore.
    ///
    class Semaphore {
    public:
        Semaphore() noexcept = default;

        ///
        /// Create/Import a semaphore.
        ///
        /// @param device Vulkan device
        /// @param fd Optional file descriptor to import the semaphore from.
        ///
        /// @throws VK::vulkan_error if object creation fails.
        ///
        Semaphore(const Device& device, std::optional<int> fd = std::nullopt);

        /// Get the Vulkan handle.
        [[nodiscard]] auto handle() const { return *this->semaphore; }

        // Trivially copyable, moveable and destructible
        Semaphore(const Semaphore&) noexcept = default;
        Semaphore& operator=(const Semaphore&) noexcept = default;
        Semaphore(Semaphore&&) noexcept = default;
        Semaphore& operator=(Semaphore&&) noexcept = default;
        ~Semaphore() = default;
    private:
        std::shared_ptr<VkSemaphore> semaphore;
    };

}
