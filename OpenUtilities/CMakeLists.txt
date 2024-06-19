cmake_minimum_required(VERSION 3.27)

project(OpenUtilities)

add_definitions(-DOPENUTILITIES_EXPORTS)

set(plugin_sdk
    "../plugin_sdk/clipper.cpp"
    "../plugin_sdk/clipper.h"
    "../plugin_sdk/cpplinq.h"
    "../plugin_sdk/plugin_sdk.cpp"
    "../plugin_sdk/plugin_sdk.hpp"
)

set(src__Utilities
    "utilities.cpp"
    "utilities.h"
)

# Prediction Library
add_library(OpenUtilities SHARED
    ${plugin_sdk}
    "dllmain.cpp"
    ${src__Utilities}
)

set_target_properties(OpenUtilities PROPERTIES
    PREFIX ""
    OUTPUT_NAME "OpenUtilities"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../"
)