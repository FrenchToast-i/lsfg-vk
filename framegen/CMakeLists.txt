cmake_minimum_required(VERSION 3.10)

if(NOT LSFGVK_EXCESS_DEBUG)
    set(CMAKE_C_VISIBILITY_PRESET "hidden")
    set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
endif()

if(LSFGVK_EXCESS_DEBUG)
    add_compile_definitions(LSFGVK_EXCESS_DEBUG)
endif()

project(lsfg-vk-framegen
    DESCRIPTION "Lossless Scaling Frame Generation Backend"
    LANGUAGES C CXX)

file(GLOB SOURCES
    "src/vk/core/*.cpp"
    "src/vk/helper/*.cpp"
    "src/vk/*.cpp"
    "src/*.cpp"
    "src/thirdparty/*.c"
)

add_library(lsfg-vk-framegen STATIC ${SOURCES})

# target
set_target_properties(lsfg-vk-framegen PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)
target_include_directories(lsfg-vk-framegen SYSTEM
    PUBLIC include/thirdparty)
target_include_directories(lsfg-vk-framegen
    PUBLIC include)

# diagnostics
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(lsfg-vk-framegen PROPERTIES
        EXPORT_COMPILE_COMMANDS ON)
endif()

if(LSFGVK_EXCESS_DEBUG)
    target_compile_options(lsfg-vk-framegen PRIVATE
        -Weverything
        # disable compat c++ flags
        -Wno-pre-c++20-compat-pedantic
        -Wno-pre-c++17-compat
        -Wno-c++98-compat-pedantic
        -Wno-c++98-compat
        # disable other flags
        -Wno-missing-designated-field-initializers
        -Wno-shadow         # allow shadowing
        -Wno-switch-enum    # ignore missing cases
        -Wno-switch-default # ignore missing default
        -Wno-padded         # ignore automatic padding
        -Wno-cast-function-type-strict # for vulkan
    )

    set_target_properties(lsfg-vk-framegen PROPERTIES
        CXX_CLANG_TIDY clang-tidy)
endif()
