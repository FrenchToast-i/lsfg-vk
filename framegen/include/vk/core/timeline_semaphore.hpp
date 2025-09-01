#pragma once

#include "vk/core/device.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <memory>

namespace VK::Core {

    ///
    /// C++ wrapper class for a Vulkan timeline semaphore.
    ///
    /// This class manages the lifetime of a Vulkan timeline semaphore.
    ///
    class TimelineSemaphore {
    public:
        TimelineSemaphore() noexcept = default;

        ///
        /// Create the timeline semaphore.
        ///
        /// @param device Vulkan device
        /// @param initial Initial value of the timeline semaphore.
        ///
        /// @throws VK::vulkan_error if object creation fails.
        ///
        TimelineSemaphore(const Device& device, uint32_t initial);

        ///
        /// Signal the timeline semaphore to a specific value.
        ///
        /// @param device Vulkan device
        /// @param value The value to signal the semaphore to.
        ///
        /// @throws VK::vulkan_error if signaling fails.
        ///
        void signal(const Device& device, uint64_t value) const;

        ///
        /// Wait for the timeline semaphore to reach a specific value.
        ///
        /// @param device Vulkan device
        /// @param value The value to wait for.
        /// @param timeout The timeout in nanoseconds, or UINT64_MAX for no timeout.
        /// @returns true if the semaphore reached the value, false if it timed out.
        ///
        /// @throws VK::vulkan_error if waiting fails.
        ///
        [[nodiscard]] bool wait(const Device& device,
            uint64_t value, uint64_t timeout = UINT64_MAX) const;

        /// Get the Vulkan handle.
        [[nodiscard]] auto handle() const { return *this->semaphore; }

        // Trivially copyable, moveable and destructible
        TimelineSemaphore(const TimelineSemaphore&) noexcept = default;
        TimelineSemaphore& operator=(const TimelineSemaphore&) noexcept = default;
        TimelineSemaphore(TimelineSemaphore&&) noexcept = default;
        TimelineSemaphore& operator=(TimelineSemaphore&&) noexcept = default;
        ~TimelineSemaphore() = default;
    private:
        std::shared_ptr<VkSemaphore> semaphore;
    };

}
