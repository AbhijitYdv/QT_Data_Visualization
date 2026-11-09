# US map data: projection, bundled state outlines, finding lat/lon columns.
# The drawing itself (UsMapItem) is part of the app because it needs Qt Quick;
# this library only needs Qt Core and Qt Gui (QPainterPath), so its tests run
# without any QML.

find_package(Qt6 REQUIRED COMPONENTS Core Gui Sql) # Sql: used by dataparsers

qt_add_library(usmap STATIC
    usmapdata.h usmapdata.cpp
)

# Lower 48 states + DC from Natural Earth 1:50m (public domain),
# simplified to ~150 KB. Served at :/maps/us-states.geojson
qt_add_resources(usmap "usmap_outlines"
    PREFIX "/maps"
    BASE resources
    FILES resources/us-states.geojson
)

target_include_directories(usmap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(usmap PUBLIC cxx_std_17)
target_link_libraries(usmap PUBLIC Qt6::Core Qt6::Gui dataparsers)

option(BUILD_USMAP_TESTS "Build the US map tests (run them with ctest)" ON)
if (BUILD_USMAP_TESTS)
    add_subdirectory(tests)
endif()
