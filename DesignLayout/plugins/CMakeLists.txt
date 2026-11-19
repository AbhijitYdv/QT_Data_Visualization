# Chart plugins. Each one builds into the app's plugin folder
# (see cmake/DataScopePlugin.cmake). Add this folder AFTER qt_add_executable.

add_subdirectory(common)      # modelaccess + xyplot: shared code (+ tests)
add_subdirectory(barchart)
add_subdirectory(linechart)
add_subdirectory(areachart)
add_subdirectory(scatterplot)
add_subdirectory(histogram)
add_subdirectory(piechart)
add_subdirectory(usmap)

option(BUILD_CHART_PLUGIN_TESTS "Build the histogram / pie / map plugin tests" ON)
if (BUILD_CHART_PLUGIN_TESTS)
    add_subdirectory(tests)
endif()
