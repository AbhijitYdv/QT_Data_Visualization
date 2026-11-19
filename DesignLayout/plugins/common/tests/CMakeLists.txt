find_package(Qt6 REQUIRED COMPONENTS Test Widgets)

# SeriesBackend on its own, plus the line / area / scatter plugins loaded
# from their build folders with a real DatasetTableModel.
qt_add_executable(tst_xyplot tst_xyplot.cpp)
target_link_libraries(tst_xyplot PRIVATE xyplot pluginhost chartdata Qt6::Test Qt6::Widgets)
target_compile_definitions(tst_xyplot PRIVATE
    LINE_PLUGIN_DIR="$<TARGET_FILE_DIR:linechart_plugin>"
    AREA_PLUGIN_DIR="$<TARGET_FILE_DIR:areachart_plugin>"
    SCATTER_PLUGIN_DIR="$<TARGET_FILE_DIR:scatterplot_plugin>")
add_dependencies(tst_xyplot linechart_plugin areachart_plugin scatterplot_plugin)
add_test(NAME tst_xyplot COMMAND tst_xyplot)
set_tests_properties(tst_xyplot PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
