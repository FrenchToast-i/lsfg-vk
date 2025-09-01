#include "vk/helper/multiset.hpp"
#include "vk/core/descriptorpool.hpp"
#include "vk/core/shadermodule.hpp"
#include "vk/core/device.hpp"

#include <vector>

using namespace VK::Helper;

MultiSet MultiSetBuilder::build(
        const Core::Device& device,
        const Core::DescriptorPool& pool,
        const Core::ShaderModule& shader) const {
    MultiSet multiset{};
    multiset.sets.reserve(this->recipes.size());

    for (const auto& recipe : this->recipes)
        multiset.sets.push_back({
            .sets = Core::DescriptorSet(device, pool, shader,
                recipe.inImages, recipe.outImages, this->samplers, this->buffer),
            .readables = recipe.inImages,
            .writables = recipe.outImages
        });

    return multiset;
}
