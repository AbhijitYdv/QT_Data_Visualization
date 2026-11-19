find_package(Qt6 REQUIRED COMPONENTS Test Widgets Charts)

# Histogram, pie and map plugins loaded from their build folders, fed a
# real DatasetTableModel - same results as the old GraphController cards.
qt_add_executable(tst_chartplugins tst_chartplugins.cpp)
target_link_libraries(tst_chartplugins PRIVATE pluginhost chartdata Qt6::Test Qt6::Widgets Qt6::Charts)
target_compile_definitions(tst_chartplugins PRIVATE
    HISTOGRAM_PLUGIN_DIR="$<TARGET_FILE_DIR:histogram_plugin>"
    PIE_PLUGIN_DIR="$<TARGET_FILE_DIR:piechart_plugin>"
    MAP_PLUGIN_DIR="$<TARGET_FILE_DIR:usmap_plugin>")
add_dependencies(tst_chartplugins histogram_plugin piechart_plugin usmap_plugin)
add_test(NAME tst_chartplugins COMMAND tst_chartplugins)
set_tests_properties(tst_chartplugins PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
