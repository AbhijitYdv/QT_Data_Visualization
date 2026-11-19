# Chart plugins. Each one builds into the app's plugin folder
# (see cmake/DataScopePlugin.cmake). Add this folder AFTER qt_add_executable.

add_subdirectory(common)      # xyplot: shared X/Y backend (+ tests)
add_subdirectory(barchart)
add_subdirectory(linechart)
add_subdirectory(areachart)
add_subdirectory(scatterplot)
