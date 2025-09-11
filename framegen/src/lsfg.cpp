#include "lsfg.hpp"
#include "trans/rsrc.hpp"

using namespace LSFG;

Instance::Instance(const std::filesystem::path& dll)
        : vkd(vk, 0, false) {
    // load shaders from dll file
    const bool fp16 = vkd.supportsFP16();
    Trans::RSRC::loadResources(dll, this->registry, fp16);

    // ...
}
