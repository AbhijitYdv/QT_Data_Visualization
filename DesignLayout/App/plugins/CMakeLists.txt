# Plugin system.
#  pluginapi  - header-only interfaces; every chart plugin links only this.
#  pluginhost - PluginManager; only the app links this.
#  datascope_add_plugin() (cmake/DataScopePlugin.cmake) builds a chart plugin.

find_package(Qt6 REQUIRED COMPONENTS Core)
include(DataScopePlugin)

add_library(pluginapi INTERFACE)
target_include_directories(pluginapi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(pluginapi INTERFACE cxx_std_17)
target_link_libraries(pluginapi INTERFACE Qt6::Core)

qt_add_library(pluginhost STATIC
    pluginmanager.h pluginmanager.cpp
)
target_link_libraries(pluginhost PUBLIC pluginapi Qt6::Core)

option(BUILD_PLUGINAPI_TESTS "Build the plugin system tests (run them with ctest)" ON)
if (BUILD_PLUGINAPI_TESTS)
    add_subdirectory(tests)
endif()
