#include "vk/registry/shader_registry.hpp"

#include <stdexcept>

using namespace VK::Registry;

void ShaderRegistry::registerModule(const std::string& name, const ShaderModuleInfo& info) {
    if (modules.find(name) != modules.end()) {
        throw std::logic_error("Shader module with name '" + name + "' already exists.");
    }

    modules[name] = info;
}

const ShaderModuleInfo& ShaderRegistry::getModule(const std::string& name) const {
    auto it = modules.find(name);
    if (it == modules.end()) {
        throw std::out_of_range("No shader module with name '" + name + "' exists.");
    }

    return it->second;
}
