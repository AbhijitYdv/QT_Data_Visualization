# Plugin API: header-only interfaces shared by the app and every chart plugin.

find_package(Qt6 REQUIRED COMPONENTS Core)

add_library(pluginapi INTERFACE)
target_include_directories(pluginapi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(pluginapi INTERFACE cxx_std_17)
target_link_libraries(pluginapi INTERFACE Qt6::Core)

option(BUILD_PLUGINAPI_TESTS "Build the plugin API tests (run them with ctest)" ON)
if (BUILD_PLUGINAPI_TESTS)
    add_subdirectory(tests)
endif()
