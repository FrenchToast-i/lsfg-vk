#pragma once

#include "vk/core/device.hpp"
#include "vk/core/instance.hpp"
#include "vk/pool/shader_pool.hpp"
#include "vk/registry/shader_registry.hpp"

#include <filesystem>

namespace LSFG {

    // FIXME: device picking

    ///
    /// Lossless Scaling Frame Generation instance.
    ///
    class Instance {
    public:
        ///
        /// Create an instance.
        ///
        /// @param dll Path to the Lossless.dll file.
        ///
        /// @throws LSFG::error if lsfg creation fails.
        ///
        Instance(const std::filesystem::path& dll);

    private:
        VK::Core::Instance vk;
        VK::Core::Device vkd;

        VK::Registry::ShaderRegistry registry;
        VK::Pool::ShaderPool shaders;
    };

}
