#pragma once

#include "vk/core/descriptorpool.hpp"
#include "vk/core/shadermodule.hpp"
#include "vk/core/sampler.hpp"
#include "vk/core/device.hpp"
#include "vk/core/buffer.hpp"
#include "vk/core/image.hpp"

#include <vulkan/vulkan_core.h>

#include <memory>
#include <vector>

namespace VK::Core {

    ///
    /// C++ wrapper class for a Vulkan descriptor set.
    ///
    /// This class manages the lifetime of a Vulkan descriptor set.
    ///
    class DescriptorSet {
    public:
        DescriptorSet() noexcept = default;

        ///
        /// Create the descriptor set.
        ///
        /// @param device Vulkan device
        /// @param pool Descriptor pool to allocate from
        /// @param shaderModule Shader module this descriptor is for
        /// @param sampledImages Sampled images to bind
        /// @param storageImages Storage images to bind
        /// @param samplers Samplers to bind
        /// @param buffer Buffer to bind
        ///
        /// @throws VK::vulkan_error if object creation fails.
        ///
        DescriptorSet(const Device& device,
            const DescriptorPool& pool, const ShaderModule& shaderModule,
            const std::vector<std::optional<Core::Image>>& sampledImages,
            const std::vector<Core::Image>& storageImages,
            const std::vector<Core::Sampler>& samplers,
            const std::optional<Core::Buffer>& buffer);

        /// Get the Vulkan handle.
        [[nodiscard]] auto handle() const { return *this->descriptorSet; }
    private:
        std::shared_ptr<VkDescriptorSet> descriptorSet;
    };

}
