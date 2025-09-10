#include "vk/pool/shader_pool.hpp"
#include "vk/registry/shader_registry.hpp"
#include "vk/core/shadermodule.hpp"
#include "vk/core/pipeline.hpp"

using namespace VK::Pool;

const ShaderInstance& ShaderPool::getOrCreate(
        const Core::Device& device,
        const Registry::ShaderRegistry& registry,
        const std::string& name) {
    const auto it = shaders.find(name);
    if (it != shaders.end()) {
        return it->second;
    }

    // create shader module
    const Registry::ShaderModuleInfo& info = registry.getModule(name);

    Core::ShaderModule shaderModule(device, info.code,
        info.sampledImages, info.storageImages, info.buffers, info.sampledImages);
    Core::Pipeline pipeline(device, shaderModule);

    const auto i = shaders.emplace(name,
        std::make_pair(std::move(shaderModule), std::move(pipeline)));
    return i.first->second;
}
