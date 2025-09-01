#include <volk.h>
#include <vulkan/vulkan_core.h>

#include "vk/core/shadermodule.hpp"
#include "vk/core/device.hpp"
#include "vk/exception.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>

using namespace VK::Core;

ShaderModule::ShaderModule(const Device& device, const std::vector<uint8_t>& code,
        size_t sampledImages,
        size_t storageImages,
        size_t buffers,
        size_t samplers) {
    // create shader module
    const uint8_t* data_ptr = code.data();
    const VkShaderModuleCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code.size(),
        .pCode = reinterpret_cast<const uint32_t*>(data_ptr)
    };
    VkShaderModule shaderModuleHandle{};
    auto res = vkCreateShaderModule(device.handle(), &createInfo, nullptr, &shaderModuleHandle);
    if (res != VK_SUCCESS || !shaderModuleHandle)
        throw VK::vulkan_error(res, "Failed to create shader module");

    // create descriptor set layout
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
    layoutBindings.reserve(buffers + samplers + sampledImages + storageImages);

    for (size_t i = 0; i < buffers; i++)
        layoutBindings.push_back({
            .binding = static_cast<uint32_t>(i),
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT
        });

    for (size_t i = 0; i < samplers; i++)
        layoutBindings.push_back({
            .binding = static_cast<uint32_t>(i + 16),
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT
        });

    for (size_t i = 0; i < sampledImages; i++)
        layoutBindings.push_back({
            .binding = static_cast<uint32_t>(i + 32),
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT
        });

    for (size_t i = 0; i < storageImages; i++)
        layoutBindings.push_back({
            .binding = static_cast<uint32_t>(i + 48),
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT
        });

    const VkDescriptorSetLayoutCreateInfo layoutDesc{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = static_cast<uint32_t>(layoutBindings.size()),
        .pBindings = layoutBindings.data()
    };
    VkDescriptorSetLayout descriptorSetLayout{};
    res = vkCreateDescriptorSetLayout(device.handle(), &layoutDesc, nullptr, &descriptorSetLayout);
    if (res != VK_SUCCESS || !descriptorSetLayout)
        throw VK::vulkan_error(res, "Failed to create descriptor set layout");

    // store module and layout in shared ptr
    this->shaderModule = std::shared_ptr<VkShaderModule>(
        new VkShaderModule(shaderModuleHandle),
        [dev = device.handle()](VkShaderModule* shaderModuleHandle) {
            vkDestroyShaderModule(dev, *shaderModuleHandle, nullptr);
        }
    );
    this->descriptorSetLayout = std::shared_ptr<VkDescriptorSetLayout>(
        new VkDescriptorSetLayout(descriptorSetLayout),
        [dev = device.handle()](VkDescriptorSetLayout* layout) {
            vkDestroyDescriptorSetLayout(dev, *layout, nullptr);
        }
    );
}
