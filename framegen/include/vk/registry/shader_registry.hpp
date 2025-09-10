#pragma once

#include <unordered_map>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace VK::Registry {

    /// Shader module information structure.
    struct ShaderModuleInfo {
        /// SPIR-V bytecode for the shader module.
        std::vector<uint8_t> code;

        /// Number of sampled images in the shader.
        size_t sampledImages{0};
        /// Number of storage images in the shader.
        size_t storageImages{0};
        /// Number of uniform/storage buffers in the shader.
        size_t buffers{0};
        /// Number of samplers in the shader.
        size_t samplers{0};
    };

    ///
    /// Registry of shader modules and their information.
    ///
    class ShaderRegistry {
    public:
        ///
        /// Create a shader registry.
        ///
        ShaderRegistry() noexcept;

        ///
        /// Register a shader module.
        ///
        /// @param name Name of the shader module.
        /// @param info Shader module information.
        ///
        /// @throws std::logic_error if a shader module with the same name already exists.
        ///
        void registerModule(const std::string& name, const ShaderModuleInfo& info);

        ///
        /// Get a shader module by name.
        ///
        /// @param name Name of the shader module.
        /// @return Shader module information.
        ///
        /// @throws std::out_of_range if no shader module with the given name exists.
        ///
        [[nodiscard]] const ShaderModuleInfo& getModule(const std::string& name) const;
    private:
        std::unordered_map<std::string, ShaderModuleInfo> modules;
    };

}
