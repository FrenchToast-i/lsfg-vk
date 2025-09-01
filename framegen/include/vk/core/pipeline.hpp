#pragma once

#include "vk/core/shadermodule.hpp"
#include "vk/core/device.hpp"

#include <vulkan/vulkan_core.h>

#include <memory>

namespace VK::Core {

    ///
    /// C++ wrapper class for a Vulkan pipeline.
    ///
    /// This class manages the lifetime of a Vulkan pipeline.
    ///
    class Pipeline {
    public:
        Pipeline() noexcept = default;

        ///
        /// Create a compute pipeline.
        ///
        /// @param device Vulkan device
        /// @param shader Shader module to use for the pipeline.
        ///
        /// @throws VK::vulkan_error if object creation fails.
        ///
        Pipeline(const Device& device, const ShaderModule& shader);

        /// Get the Vulkan handle.
        [[nodiscard]] auto handle() const { return *this->pipeline; }
        /// Get the pipeline layout.
        [[nodiscard]] auto getLayout() const { return *this->layout; }
    private:
        std::shared_ptr<VkPipeline> pipeline;
        std::shared_ptr<VkPipelineLayout> layout;
    };

}
