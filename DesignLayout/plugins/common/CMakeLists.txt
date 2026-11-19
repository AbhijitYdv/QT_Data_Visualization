# Code shared by the chart plugins.
#  modelaccess - header-only helpers for reading the dataset model by role name
#  xyplot      - X/Y backend (bar, line, area, scatter). Static, so each plugin
#                carries its own copy and stays a single file.

find_package(Qt6 REQUIRED COMPONENTS Core Charts)

add_library(modelaccess INTERFACE)
target_include_directories(modelaccess INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(modelaccess INTERFACE Qt6::Core)

qt_add_library(xyplot STATIC
    xybackend.h xybackend.cpp
    seriesbackend.h seriesbackend.cpp
)
target_include_directories(xyplot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(xyplot PUBLIC cxx_std_17)
# Linked into several shared plugins: position-independent, and every symbol
# hidden so each plugin keeps its own private copy (see DataScopePlugin.cmake).
set_target_properties(xyplot PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON)
target_link_libraries(xyplot PUBLIC Qt6::Core Qt6::Charts)

option(BUILD_XYPLOT_TESTS "Build the X/Y chart plugin tests (run them with ctest)" ON)
if (BUILD_XYPLOT_TESTS)
    add_subdirectory(tests)
endif()
