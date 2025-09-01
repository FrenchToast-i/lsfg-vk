#include <volk.h>
#include <vulkan/vulkan_core.h>

#include "vk/core/sampler.hpp"
#include "vk/core/device.hpp"
#include "vk/exception.hpp"

#include <memory>

using namespace VK::Core;

Sampler::Sampler(const Device& device,
        VkSamplerAddressMode mode, VkCompareOp compare, bool isWhite) {
    // create sampler
    const VkSamplerCreateInfo desc{
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = mode,
        .addressModeV = mode,
        .addressModeW = mode,
        .compareOp = compare,
        .maxLod = VK_LOD_CLAMP_NONE,
        .borderColor =
            isWhite ? VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE
                    : VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK
    };
    VkSampler samplerHandle{};
    auto res = vkCreateSampler(device.handle(), &desc, nullptr, &samplerHandle);
    if (res != VK_SUCCESS || samplerHandle == VK_NULL_HANDLE)
        throw VK::vulkan_error(res, "Unable to create sampler");

    // store sampler in shared ptr
    this->sampler = std::shared_ptr<VkSampler>(
        new VkSampler(samplerHandle),
        [dev = device.handle()](VkSampler* samplerHandle) {
            vkDestroySampler(dev, *samplerHandle, nullptr);
        }
    );
}
