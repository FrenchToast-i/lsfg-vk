#include <volk.h>
#include <vulkan/vulkan_core.h>

#include "vk/core/descriptorpool.hpp"
#include "vk/core/descriptorset.hpp"
#include "vk/core/shadermodule.hpp"
#include "vk/core/sampler.hpp"
#include "vk/core/buffer.hpp"
#include "vk/core/device.hpp"
#include "vk/core/image.hpp"
#include "vk/exception.hpp"

#include <optional>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

using namespace VK::Core;

DescriptorSet::DescriptorSet(const Device& device,
        const DescriptorPool& pool, const ShaderModule& shaderModule,
        const std::vector<std::optional<Core::Image>>& sampledImages,
        const std::vector<Core::Image>& storageImages,
        const std::vector<Core::Sampler>& samplers,
        const std::optional<Core::Buffer>& buffer) {
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

    const size_t bindingCount = samplers.size() + sampledImages.size()
        + storageImages.size() + (buffer.has_value() ? 1 : 0);

    // create descriptor writes
    std::vector<VkWriteDescriptorSet> entries;
    entries.reserve(bindingCount);

    std::optional<VkDescriptorBufferInfo> bufferInfos;

    if (buffer.has_value())
        entries.push_back({
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSetHandle,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &(bufferInfos = VkDescriptorBufferInfo{
                .buffer = buffer->handle(),
                .range = buffer->getSize()
            }).value()
        });

    std::vector<VkDescriptorImageInfo> imageInfos;
    imageInfos.reserve(bindingCount);

    size_t samplerIdx{16};
    for (const auto& samp : samplers)
        entries.push_back({
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSetHandle,
            .dstBinding = static_cast<uint32_t>(samplerIdx++),
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
            .pImageInfo = &(imageInfos.emplace_back(VkDescriptorImageInfo{
                .sampler = samp.handle(),
            }))
        });

    size_t sampledIdx{32};
    for (const auto& img : sampledImages) {
        entries.push_back({
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSetHandle,
            .dstBinding = static_cast<uint32_t>(sampledIdx++),
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .pImageInfo = &(imageInfos.emplace_back(VkDescriptorImageInfo{
                .imageView = img.has_value() ? img->getView() : nullptr,
                .imageLayout = img.has_value() ?
                    VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_UNDEFINED
            }))
        });
    }

    size_t storageIdx{48};
    for (const auto& img : storageImages)
        entries.push_back({
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptorSetHandle,
            .dstBinding = static_cast<uint32_t>(storageIdx++),
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .pImageInfo = &(imageInfos.emplace_back(VkDescriptorImageInfo{
                .imageView = img.getView(),
                .imageLayout = VK_IMAGE_LAYOUT_GENERAL
            }))
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
