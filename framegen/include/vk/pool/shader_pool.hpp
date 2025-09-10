#pragma once

#include "vk/core/device.hpp"
#include "vk/core/pipeline.hpp"
#include "vk/core/shadermodule.hpp"
#include "vk/registry/shader_registry.hpp"

#include <string>
#include <utility>

namespace VK::Pool {

    using ShaderInstance = std::pair<Core::ShaderModule, Core::Pipeline>;

    ///
    /// Pool of initialized shader modules and pipelines.
    ///
    class ShaderPool {
    public:
        ///
        /// Create a shader pool.
        ///
        ShaderPool() noexcept = default;

        ///
        /// Get or create a shader module and pipeline.
        ///
        /// @param device Vulkan device
        /// @param registry Shader registry
        /// @param name Name of the shader module
        /// @return Pair of shader module and pipeline
        ///
        /// @throws std::out_of_range if no such shader exists in the registry.
        /// @throws VK::vulkan_error if shader module or pipeline creation fails.
        ///
        [[nodiscard]] const ShaderInstance& getOrCreate(
            const Core::Device& device,
            const Registry::ShaderRegistry& registry,
            const std::string& name);
    private:
        std::unordered_map<std::string, ShaderInstance> shaders;
    };

}
