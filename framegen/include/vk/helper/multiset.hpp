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
        [[nodiscard]] const auto& at(size_t index) const {
            return this->set(index).sets;
        }
        /// Get the readable images for a specific frame index.
        [[nodiscard]] const auto& readablesAt(size_t index) const {
            return this->set(index).readables;
        }
        /// Get the writable images for a specific frame index.
        [[nodiscard]] const auto& writablesAt(size_t index) const {
            return this->set(index).writables;
        }
    private:
        struct Set {
            Core::DescriptorSet sets;
            std::vector<std::optional<Core::Image>> readables;
            std::vector<Core::Image> writables;
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

        /// Add an input image to the descriptor.
        MultiSetBuilder& addInput(const Core::Image& image) {
            for (auto& recipe : this->recipes)
                recipe.inImages.emplace_back(image);
            return *this;
        }
        /// Add an optional input image to the descriptor.
        MultiSetBuilder& addInput(const std::optional<Core::Image>& image) {
            for (auto& recipe : this->recipes)
                recipe.inImages.emplace_back(image);
            return *this;
        }
        /// Add multiple input images to the descriptor.
        MultiSetBuilder& addInput(const std::vector<Core::Image>& images) {
            for (auto& recipe : this->recipes)
                for (const auto& img : images)
                    recipe.inImages.emplace_back(img);
            return *this;
        }

        /// Add a temporal input image to the descriptor.
        MultiSetBuilder& addInput(const TemporalImage& temporalImage, size_t offset = 0) {
            for (auto& recipe : this->recipes)
                recipe.inImages.emplace_back(temporalImage.at(offset++));
            return *this;
        }

        /// Add a group of input images to the descriptor.
        MultiSetBuilder& addInput(const ImageGroup& imageGroup) {
            this->addInput(imageGroup.into());
            return *this;
        }
        /// Add a subgroup of input images to the descriptor.
        MultiSetBuilder& addInput(const ImageGroup& imageGroup, size_t start, size_t length) {
            this->addInput(imageGroup.subGroup(start, length));
            return *this;
        }

        /// Add all mipmapped images to the descriptor.
        MultiSetBuilder& addInput(const Helper::MipmappedImage& mipmappedImage) {
            this->addInput(mipmappedImage.into());
            return *this;
        }

        /// Add an output image to the descriptor.
        MultiSetBuilder& addOutput(const Core::Image& image) {
            for (auto& recipe : this->recipes)
                recipe.outImages.push_back(image);
            return *this;
        }
        /// Add multiple output images to the descriptor.
        MultiSetBuilder& addOutput(const std::vector<Core::Image>& images) {
            for (auto& recipe : this->recipes)
                for (const auto& img : images)
                    recipe.outImages.push_back(img);
            return *this;
        }

        /// Add a temporal output image to the descriptor.
        MultiSetBuilder& addOutput(const TemporalImage& temporalImage, size_t offset = 0) {
            for (auto& recipe : this->recipes)
                recipe.outImages.push_back(temporalImage.at(offset++));
            return *this;
        }

        /// Add a group of output images to the descriptor.
        MultiSetBuilder& addOutput(const ImageGroup& imageGroup) {
            this->addOutput(imageGroup.into());
            return *this;
        }
        /// Add a subgroup of output images to the descriptor.
        MultiSetBuilder& addOutput(const ImageGroup& imageGroup, size_t start, size_t length) {
            this->addOutput(imageGroup.subGroup(start, length));
            return *this;
        }

        /// Add all mipmapped images to the descriptor.
        MultiSetBuilder& addOutput(const Helper::MipmappedImage& mipmappedImage) {
            this->addOutput(mipmappedImage.into());
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
            std::vector<std::optional<Core::Image>> inImages;
            std::vector<Core::Image> outImages;
        };
        std::vector<SetRecipe> recipes;
    };

}
