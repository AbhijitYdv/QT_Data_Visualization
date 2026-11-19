find_package(Qt6 REQUIRED COMPONENTS Test)

# Loads the built plugin from its build folder (like the app does from
# its plugin folder) and feeds it a real DatasetTableModel.
qt_add_executable(tst_barchart tst_barchart.cpp)
target_link_libraries(tst_barchart PRIVATE pluginhost chartdata Qt6::Test)
target_compile_definitions(tst_barchart PRIVATE
    BAR_PLUGIN_DIR="$<TARGET_FILE_DIR:barchart_plugin>")
add_dependencies(tst_barchart barchart_plugin)
add_test(NAME tst_barchart COMMAND tst_barchart)
