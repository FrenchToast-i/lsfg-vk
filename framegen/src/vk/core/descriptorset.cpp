#include <volk.h>
#include <vulkan/vulkan_core.h>

#include "vk/core/descriptorpool.hpp"
#include "vk/core/descriptorset.hpp"
#include "vk/core/shadermodule.hpp"
#include "vk/core/device.hpp"
#include "vk/exception.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

using namespace VK::Core;

DescriptorSet::DescriptorSet(const Device& device,
        const DescriptorPool& pool, const ShaderModule& shaderModule,
        const std::vector<ImageInfo>& sampledImages,
        const std::vector<ImageInfo>& storageImages,
        const std::vector<SamplerInfo>& samplers,
        const std::vector<BufferInfo>& uniformBuffers) {
    // create descriptor set
    VkDescriptorSetLayout layout = shaderModule.getLayout();
    const VkDescriptorSetAllocateInfo desc{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = pool.handle(),
        .descriptorSetCount = 1,
        .pSetLayouts = &layout
    };
    VkDescriptorSet descriptorSetHandle{};
    auto res = vkAllocateDescriptorSets(device.handle(), &desc, &descriptorSetHandle);
    if (res != VK_SUCCESS || descriptorSetHandle == VK_NULL_HANDLE)
        throw VK::vulkan_error(res, "Unable to allocate descriptor set");

    // create descriptor writes
    const size_t totalEntries =
        storageImages.size() + samplers.size() + uniformBuffers.size() + sampledImages.size();
    std::vector<VkWriteDescriptorSet> entries(totalEntries);

    size_t bufferIdx{0};
    for (const auto& buf : uniformBuffers)
        entries.push_back({
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSetHandle,
            .dstBinding = static_cast<uint32_t>(bufferIdx++),
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = buf.handle()
        });

    size_t samplerIdx{16};
    for (const auto& samp : samplers)
        entries.push_back({
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSetHandle,
            .dstBinding = static_cast<uint32_t>(samplerIdx++),
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
            .pImageInfo = samp.handle()
        });

    size_t inputIdx{32};
    for (const auto& img : sampledImages)
        entries.push_back({
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSetHandle,
            .dstBinding = static_cast<uint32_t>(inputIdx++),
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .pImageInfo = img.handle()
        });

    size_t outputIdx{48};
    for (const auto& img : storageImages)
        entries.push_back({
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSetHandle,
            .dstBinding = static_cast<uint32_t>(outputIdx++),
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .pImageInfo = img.handle()
        });

    // update descriptor set
    vkUpdateDescriptorSets(device.handle(),
        static_cast<uint32_t>(entries.size()), entries.data(), 0, nullptr);

    // store set in shared ptr
    this->descriptorSet = std::shared_ptr<VkDescriptorSet>(
        new VkDescriptorSet(descriptorSetHandle),
        [dev = device.handle(), pool = pool](VkDescriptorSet* setHandle) {
            vkFreeDescriptorSets(dev, pool.handle(), 1, setHandle);
        }
    );
}
