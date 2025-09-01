#pragma once

#include "vk/core/device.hpp"

#include <vulkan/vulkan_core.h>

#include <cstddef>
#include <memory>

namespace VK::Core {

    ///
    /// C++ wrapper class for a Vulkan buffer.
    ///
    /// This class manages the lifetime of a Vulkan buffer.
    ///
    class Buffer {
    public:
        Buffer() noexcept = default;

        ///
        /// Create the buffer.
        ///
        /// @param device Vulkan device
        /// @param data Initial data for the buffer, also specifies the size of the buffer.
        /// @param usage Usage flags for the buffer
        ///
        /// @throws VK::vulkan_error if object creation fails.
        ///
        template<typename T>
        Buffer(const Device& device, const T& data, VkBufferUsageFlags usage)
            : Buffer(device, reinterpret_cast<const void*>(&data), sizeof(T), usage) {}

        ///
        /// Create the buffer.
        ///
        /// @param device Vulkan device
        /// @param data Initial data for the buffer
        /// @param size Size of the buffer in bytes
        /// @param usage Usage flags for the buffer
        ///
        /// @throws VK::vulkan_error if object creation fails.
        ///
        Buffer(const Device& device, const void* data, size_t size, VkBufferUsageFlags usage);

        /// Get the Vulkan handle.
        [[nodiscard]] auto handle() const { return *this->buffer; }
        /// Get the Vulkan device memory handle.
        [[nodiscard]] auto getMemory() const { return *this->memory; }
        /// Get the size of the buffer.
        [[nodiscard]] auto getSize() const { return this->size; }

        /// Trivially copyable, moveable and destructible
        Buffer(const Buffer&) noexcept = default;
        Buffer& operator=(const Buffer&) noexcept = default;
        Buffer(Buffer&&) noexcept = default;
        Buffer& operator=(Buffer&&) noexcept = default;
        ~Buffer() = default;
    private:

        std::shared_ptr<VkBuffer> buffer;
        std::shared_ptr<VkDeviceMemory> memory;

        size_t size{};
    };

}
