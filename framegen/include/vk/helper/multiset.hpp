#pragma once

#include "vk/helper/mipmapped_image.hpp"
#include "vk/helper/temporal_image.hpp"
#include "vk/helper/image_group.hpp"
#include "vk/core/descriptorpool.hpp"
#include "vk/core/descriptorset.hpp"
#include "vk/core/shadermodule.hpp"
#include "vk/core/sampler.hpp"
#include "vk/core/buffer.hpp"
#include "vk/core/image.hpp"

#include <optional>
#include <cstddef>
#include <vector>

namespace VK::Helper {

    ///
    /// A list of descriptor sets across multiple frames.
    ///
    class MultiSet {
        friend class MultiSetBuilder;
    public:
        MultiSet() noexcept = default;

        /// Get the amount of descriptor sets.
        [[nodiscard]] auto size() const noexcept { return this->sets.size(); }
        /// Get all the descriptor sets.
        [[nodiscard]] const auto& all() const { return this->sets; }

        /// Get the descriptor set for a specific frame index.
        [[nodiscard]] const auto& getSetAt(size_t index) const {
            return this->set(index).sets;
        }
        /// Get the sampled images for a specific frame index.
        [[nodiscard]] const auto& getSampledImagesAt(size_t index) const {
            return this->set(index).sampled;
        }
        /// Get the storage images for a specific frame index.
        [[nodiscard]] const auto& getStorageImagesAt(size_t index) const {
            return this->set(index).storage;
        }
    private:
        struct Set {
            Core::DescriptorSet sets;
            std::vector<std::optional<Core::Image>> sampled;
            std::vector<Core::Image> storage;
        };
        std::vector<Set> sets;

        /// Get the set at a specific index.
        [[nodiscard]] const Set& set(size_t index) const {
            return this->sets.at(index % this->sets.size());
        }
    };

    ///
    /// Builder class for a shader descriptors across multiple frames.
    ///
    class MultiSetBuilder {
    public:
        MultiSetBuilder() noexcept = default;

        ///
        /// Create a new descriptor builder.
        ///
        /// @param count Amount of sets to create.
        ///
        MultiSetBuilder(size_t count) noexcept : recipes(count) {}

        /// Add a buffer to the descriptor.
        MultiSetBuilder& withBuffer(const Core::Buffer& buffer) {
            this->buffer = buffer;
            return *this;
        }

        /// Add a sampler to the descriptor.
        MultiSetBuilder& withSampler(const Core::Sampler& sampler) {
            this->samplers = { sampler };
            return *this;
        }
        /// Add two samplers to the descriptor.
        MultiSetBuilder& withSamplers(
                const Core::Sampler& sampler1, const Core::Sampler& sampler2) {
            this->samplers = { sampler1, sampler2 };
            return *this;
        }

        /// Add a sampled image to the descriptor.
        MultiSetBuilder& addSampledImage(const Core::Image& image) {
            for (auto& recipe : this->recipes)
                recipe.sampled.emplace_back(image);
            return *this;
        }
        /// Add an optional sampled image to the descriptor.
        MultiSetBuilder& addSampledImage(const std::optional<Core::Image>& image) {
            for (auto& recipe : this->recipes)
                recipe.sampled.emplace_back(image);
            return *this;
        }
        /// Add multiple sampled images to the descriptor.
        MultiSetBuilder& addSampledImage(const std::vector<Core::Image>& images) {
            for (auto& recipe : this->recipes)
                for (const auto& img : images)
                    recipe.sampled.emplace_back(img);
            return *this;
        }

        /// Add a temporal sampled image to the descriptor.
        MultiSetBuilder& addSampledImage(const TemporalImage& temporalImage, size_t offset = 0) {
            for (auto& recipe : this->recipes)
                recipe.sampled.emplace_back(temporalImage.at(offset++));
            return *this;
        }

        /// Add a group of sampled images to the descriptor.
        MultiSetBuilder& addSampledImage(const ImageGroup& imageGroup) {
            this->addSampledImage(imageGroup.into());
            return *this;
        }
        /// Add a subgroup of sampled images to the descriptor.
        MultiSetBuilder& addSampledImage(const ImageGroup& imageGroup, size_t start, size_t length) {
            this->addSampledImage(imageGroup.subGroup(start, length));
            return *this;
        }

        /// Add all mipmapped images to the descriptor.
        MultiSetBuilder& addSampledImage(const Helper::MipmappedImage& mipmappedImage) {
            this->addSampledImage(mipmappedImage.into());
            return *this;
        }

        /// Add a storage image to the descriptor.
        MultiSetBuilder& addStorageImage(const Core::Image& image) {
            for (auto& recipe : this->recipes)
                recipe.storage.push_back(image);
            return *this;
        }
        /// Add multiple storage images to the descriptor.
        MultiSetBuilder& addStorageImage(const std::vector<Core::Image>& images) {
            for (auto& recipe : this->recipes)
                for (const auto& img : images)
                    recipe.storage.push_back(img);
            return *this;
        }

        /// Add a temporal storage image to the descriptor.
        MultiSetBuilder& addStorageImage(const TemporalImage& temporalImage, size_t offset = 0) {
            for (auto& recipe : this->recipes)
                recipe.storage.push_back(temporalImage.at(offset++));
            return *this;
        }

        /// Add a group of storage images to the descriptor.
        MultiSetBuilder& addStorageImage(const ImageGroup& imageGroup) {
            this->addStorageImage(imageGroup.into());
            return *this;
        }
        /// Add a subgroup of storage images to the descriptor.
        MultiSetBuilder& addStorageImage(const ImageGroup& imageGroup, size_t start, size_t length) {
            this->addStorageImage(imageGroup.subGroup(start, length));
            return *this;
        }

        /// Add all mipmapped images to the descriptor.
        MultiSetBuilder& addStorageImage(const Helper::MipmappedImage& mipmappedImage) {
            this->addStorageImage(mipmappedImage.into());
            return *this;
        }

        ///
        /// Build the descriptor sets.
        ///
        /// @param device The Vulkan device.
        /// @param descriptorPool The descriptor pool to allocate from.
        /// @param shaderModule The shader module this descriptor is for.
        /// @return The built descriptor sets.
        ///
        [[nodiscard]] MultiSet build(
            const Core::Device& device,
            const Core::DescriptorPool& descriptorPool,
            const Core::ShaderModule& shaderModule) const;

    private:
        std::optional<Core::Buffer> buffer;
        std::vector<Core::Sampler> samplers;
        struct SetRecipe {
            std::vector<std::optional<Core::Image>> sampled;
            std::vector<Core::Image> storage;
        };
        std::vector<SetRecipe> recipes;
    };

}
