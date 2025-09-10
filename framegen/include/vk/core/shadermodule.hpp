#pragma once

#include "vk/core/device.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <cstddef>
#include <vector>
#include <memory>

namespace VK::Core {

    ///
    /// C++ wrapper class for a Vulkan shader module.
    ///
    /// This class manages the lifetime of a Vulkan shader module.
    ///
    class ShaderModule {
    public:
        ///
        /// Create the shader module.
        ///
        /// @param device Vulkan device
        /// @param code SPIR-V bytecode for the shader.
        /// @param sampledImages Number of sampled images in the shader.
        /// @param storageImages Number of storage images in the shader.
        /// @param buffers Number of uniform/storage buffers in the shader.
        /// @param samplers Number of samplers in the shader.
        ///
        /// @throws VK::vulkan_error if object creation fails.
        ///
        ShaderModule(const Device& device, const std::vector<uint8_t>& code,
            size_t sampledImages,
            size_t storageImages,
            size_t buffers,
            size_t samplers);

        /// Get the Vulkan handle.
        [[nodiscard]] auto handle() const { return *this->shaderModule; }
        /// Get the descriptor set layout.
        [[nodiscard]] auto getLayout() const { return *this->descriptorSetLayout; }
    private:
        std::shared_ptr<VkShaderModule> shaderModule;
        std::shared_ptr<VkDescriptorSetLayout> descriptorSetLayout;
    };

}
