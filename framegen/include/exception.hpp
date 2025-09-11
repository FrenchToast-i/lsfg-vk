#pragma once

#include <vulkan/vulkan_core.h>

#include <stdexcept>
#include <string>

namespace LSFG {

    /// Simple exception class for stacking errors.
    class error : public std::runtime_error {
    public:
        ///
        /// Construct a new error with a message.
        ///
        /// @param message The error message.
        ///
        explicit error(const std::string& message);

        ///
        /// Construct a new error with a message.
        ///
        /// @param message The error message.
        /// @param exe The original exception to rethrow.
        ///
        explicit error(const std::string& message,
            const std::exception& exe);

        /// Get the exception as a string.
        [[nodiscard]] const char* what() const noexcept override {
            return message.c_str();
        }

        // Trivially copyable, moveable and destructible
        error(const error&) = default;
        error(error&&) = default;
        error& operator=(const error&) = default;
        error& operator=(error&&) = default;
        ~error() noexcept override;
    private:
        std::string message;
    };


}
