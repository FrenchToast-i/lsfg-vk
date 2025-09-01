#pragma once

#include "vk/core/instance.hpp"

#include <vulkan/vulkan_core.h>

#include <optional>
#include <cstdint>
#include <bitset>
#include <memory>

namespace VK::Core {

    // FIXME: More intelligent device choosing method. See #200.

    ///
    /// C++ wrapper class for a Vulkan device.
    ///
    /// This class manages the lifetime of a Vulkan device.
    ///
    class Device {
    public:
        Device() noexcept = default;

        ///
        /// Create the device.
        ///
        /// @param instance Vulkan instance
        /// @param uuid The UUID of the Vulkan device to use.
        /// @param enforceFP32 Whether to enforce FP32 support even if FP16 is available.
        ///
        /// @throws VK::vulkan_error if object creation fails.
        ///
        Device(const Instance& instance, uint64_t uuid, bool enforceFP32);

        ///
        /// Find a suitable memory type.
        ///
        /// @param validTypes A bitset representing the valid memory types.
        /// @param hostVisible Whether the memory should be host visible.
        ///
        /// @return The index of a suitable memory type, or std::nullopt if none found.
        ///
        [[nodiscard]] std::optional<uint32_t> findMemoryType(
            std::bitset<32> validTypes, bool hostVisible = false) const;

        /// Get the Vulkan handle.
        [[nodiscard]] auto handle() const { return *this->device; }

        /// Get the physical device associated with this logical device.
        [[nodiscard]] auto getPhysicalDevice() const { return this->physicalDevice; }
        /// Get the compute queue.
        [[nodiscard]] auto getComputeQueue() const { return this->computeQueue; }

        /// Get the compute queue family index.
        [[nodiscard]] auto getComputeFamilyIdx() const { return this->computeFamilyIdx; }
        /// Check if the device supports FP16.
        [[nodiscard]] auto supportsFP16() const { return this->fp16; }
    private:
        std::shared_ptr<VkDevice> device;

        VkPhysicalDevice physicalDevice{};
        VkQueue computeQueue{};

        uint32_t computeFamilyIdx{0};
        bool fp16{false};

    };

}
