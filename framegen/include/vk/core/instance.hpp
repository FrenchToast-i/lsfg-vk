#pragma once

#include <vulkan/vulkan_core.h>

#include <memory>

namespace VK::Core {

    ///
    /// C++ wrapper class for a Vulkan instance.
    ///
    /// This class manages the lifetime of a Vulkan instance.
    ///
    class Instance {
    public:
        ///
        /// Create the instance.
        ///
        /// @throws VK::vulkan_error if object creation fails.
        ///
        Instance();

        /// Get the Vulkan handle.
        [[nodiscard]] auto handle() const { return *this->instance; }
    private:
        std::shared_ptr<VkInstance> instance;
    };

}
