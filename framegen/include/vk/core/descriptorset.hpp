#pragma once

#include "vk/core/descriptorpool.hpp"
#include "vk/core/shadermodule.hpp"
#include "vk/core/sampler.hpp"
#include "vk/core/device.hpp"
#include "vk/core/buffer.hpp"
#include "vk/core/image.hpp"

#include <vulkan/vulkan_core.h>

#include <memory>
#include <vector>

namespace VK::Core {

    /// Helper class to wrap VkDescriptorImageInfo
    class ImageInfo {
    public:
        ImageInfo() noexcept = default; // skipping images is allowed

        ImageInfo(const Image& image) noexcept
            : info{
                .imageView = image.getView(),
                .imageLayout = VK_IMAGE_LAYOUT_GENERAL
            } {}

        /// Get the Vulkan handle.
        [[nodiscard]] auto handle() const { return &this->info; }
    private:
        VkDescriptorImageInfo info{};
    };

    /// Helper class to wrap VkDescriptorImageInfo for samplers
    class SamplerInfo {
    public:
        SamplerInfo(const Sampler& sampler) noexcept
            : info{
                .sampler = sampler.handle(),
            } {}

        /// Get the Vulkan handle.
        [[nodiscard]] auto handle() const { return &this->info; }
    private:
        VkDescriptorImageInfo info{};
    };

    /// Helper class to wrap VkDescriptorBufferInfo
    class BufferInfo {
    public:
        BufferInfo(const Buffer& buffer) noexcept
            : info{
                .buffer = buffer.handle(),
                .range = buffer.getSize()
            } {}

        /// Get the Vulkan handle.
        [[nodiscard]] auto handle() const { return &this->info; }
    private:
        VkDescriptorBufferInfo info{};
    };

    ///
    /// C++ wrapper class for a Vulkan descriptor set.
    ///
    /// This class manages the lifetime of a Vulkan descriptor set.
    ///
    class DescriptorSet {
    public:
        DescriptorSet() noexcept = default;

        ///
        /// Create the descriptor set.
        ///
        /// @param device Vulkan device
        /// @param pool Descriptor pool to allocate from
        /// @param shaderModule Shader module to use for the descriptor set
        ///
        /// @throws VK::vulkan_error if object creation fails.
        ///
        DescriptorSet(const Device& device,
            const DescriptorPool& pool, const ShaderModule& shaderModule,
            const std::vector<ImageInfo>& sampledImages,
            const std::vector<ImageInfo>& storageImages,
            const std::vector<SamplerInfo>& samplers,
            const std::vector<BufferInfo>& uniformBuffers);

        /// Get the Vulkan handle.
        [[nodiscard]] auto handle() const { return *this->descriptorSet; }
    private:
        std::shared_ptr<VkDescriptorSet> descriptorSet;
    };

}
