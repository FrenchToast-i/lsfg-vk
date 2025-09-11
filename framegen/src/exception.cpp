#include "exception.hpp"

#include <stdexcept>
#include <format>
#include <string>

using namespace LSFG;

error::error(const std::string& message)
        : std::runtime_error(message), message(message) {}

error::error(const std::string& message, const std::exception& exe)
        : std::runtime_error(message) {
    this->message = std::format("{}\n- {}", message, exe.what());
}

error::~error() noexcept = default;
