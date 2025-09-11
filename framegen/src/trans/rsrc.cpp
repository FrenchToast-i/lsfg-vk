#include "trans/rsrc.hpp"
#include "exception.hpp"
#include "trans/dll.hpp"
#include "vk/registry/shader_registry.hpp"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

using namespace Trans;

namespace {
    const size_t OFFSET_FP16 = 49;

    std::vector<uint8_t> get(
            const std::unordered_map<uint32_t, std::vector<uint8_t>>& source,
            uint32_t id,
            bool fp16) {
        auto it = source.find(id + (fp16 ? OFFSET_FP16 : 0));
        if (it == source.end())
            throw LSFG::error("Missing resource ID: " + std::to_string(id));

        return it->second;
    }
}

/*
TODO for normal mode
 { "gamma[0]", 257 + NO },
 { "gamma[1]", 259 + NO },
 { "gamma[2]", 260 + NO },
 { "gamma[3]", 261 + NO },
 { "gamma[4]", 262 + NO },
 { "delta[0]", 257 + NO },
 { "delta[1]", 263 + NO },
 { "delta[2]", 264 + NO },
 { "delta[3]", 265 + NO },
 { "delta[4]", 266 + NO },
 { "delta[5]", 258 + NO },
 { "delta[6]", 271 + NO },
 { "delta[7]", 272 + NO },
 { "delta[8]", 273 + NO },
 { "delta[9]", 274 + NO },
 { "generate", 256 + NO },
}};
 */

void RSRC::loadResources(
        const std::filesystem::path& filename,
        VK::Registry::ShaderRegistry& registry,
        bool fp16) {
    // parse dll file
    if (!std::filesystem::exists(filename))
        throw LSFG::error("DLL file does not exist: " + filename.string());

    std::unordered_map<uint32_t, std::vector<uint8_t>> rsrcs;
    try {
        rsrcs = DLL::parseDLL(filename);
    } catch (const std::runtime_error& e) {
        throw LSFG::error("Unable to parse Lossless.dll file", e);
    }

    // register resources
    registry.registerModule("mipmaps", VK::Registry::ShaderModuleInfo {
        .code = get(rsrcs, 304, fp16),
        .sampledImages = 1, .storageImages = 7,
        .buffers = 1, .samplers = 1
    });

    registry.registerModule("alpha[0]", VK::Registry::ShaderModuleInfo {
        .code = get(rsrcs, 316, fp16),
        .sampledImages = 1, .storageImages = 2,
        .samplers = 1
    });
    registry.registerModule("alpha[1]", VK::Registry::ShaderModuleInfo {
        .code = get(rsrcs, 317, fp16),
        .sampledImages = 2, .storageImages = 2,
        .samplers = 1
    });
    registry.registerModule("alpha[2]", VK::Registry::ShaderModuleInfo {
        .code = get(rsrcs, 318, fp16),
        .sampledImages = 2, .storageImages = 4,
        .samplers = 1
    });
    registry.registerModule("alpha[3]", VK::Registry::ShaderModuleInfo {
        .code = get(rsrcs, 319, fp16),
        .sampledImages = 4, .storageImages = 4,
        .samplers = 1
    });

    registry.registerModule("beta[0]", VK::Registry::ShaderModuleInfo {
        .code = get(rsrcs, 324, fp16),
        .sampledImages = 12, .storageImages = 2,
        .samplers = 1
    });
    registry.registerModule("beta[1]", VK::Registry::ShaderModuleInfo {
        .code = get(rsrcs, 325, fp16),
        .sampledImages = 2, .storageImages = 2,
        .samplers = 1
    });
    registry.registerModule("beta[2]", VK::Registry::ShaderModuleInfo {
        .code = get(rsrcs, 326, fp16),
        .sampledImages = 2, .storageImages = 2,
        .samplers = 1
    });
    registry.registerModule("beta[3]", VK::Registry::ShaderModuleInfo {
        .code = get(rsrcs, 327, fp16),
        .sampledImages = 2, .storageImages = 2,
        .samplers = 1
    });
    registry.registerModule("beta[4]", VK::Registry::ShaderModuleInfo {
        .code = get(rsrcs, 328, fp16),
        .sampledImages = 4, .storageImages = 6,
        .buffers = 1, .samplers = 1
    });
}
