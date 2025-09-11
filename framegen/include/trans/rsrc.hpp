#pragma once

#include "vk/registry/shader_registry.hpp"

#include <filesystem>

namespace Trans::RSRC {

    ///
    /// Load all resources into memory.
    ///
    /// @param filename Path to the DLL file
    /// @param registry Shader registry
    /// @param fp16 Prefer FP16 shaders
    ///
    /// @throws LSFG::error if loading fails.
    ///
    void loadResources(
        const std::filesystem::path& filename,
        VK::Registry::ShaderRegistry& registry,
        bool fp16);

}
