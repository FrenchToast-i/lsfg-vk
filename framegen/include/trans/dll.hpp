#pragma once

#include <unordered_map>
#include <filesystem>
#include <cstdint>
#include <vector>

namespace Trans::DLL {

    ///
    /// Parse all resources from a DLL file.
    ///
    /// @param filename Path to the DLL file.
    /// @return A map of resource IDs to their binary data.
    ///
    /// @throws std::runtime_error on various failure points.
    ///
    std::unordered_map<uint32_t, std::vector<uint8_t>> parseDLL(
        const std::filesystem::path& filename);

}
