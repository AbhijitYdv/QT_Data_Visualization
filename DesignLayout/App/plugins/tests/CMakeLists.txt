find_package(Qt6 REQUIRED COMPONENTS Test Gui)

# Step 1: the interface on its own
qt_add_executable(tst_pluginapi tst_pluginapi.cpp)
target_link_libraries(tst_pluginapi PRIVATE pluginapi Qt6::Test Qt6::Gui)
add_test(NAME tst_pluginapi COMMAND tst_pluginapi)

# Step 2: PluginManager loading real plugin files.
# The fixtures go into their own folder so the test controls exactly
# what PluginManager sees.
set(FIXTURE_DIR ${CMAKE_CURRENT_BINARY_DIR}/fixture_plugins)
add_subdirectory(testplugin)

qt_add_executable(tst_pluginmanager tst_pluginmanager.cpp)
target_link_libraries(tst_pluginmanager PRIVATE pluginhost Qt6::Test)
target_compile_definitions(tst_pluginmanager PRIVATE
    FIXTURE_DIR="${FIXTURE_DIR}")
add_dependencies(tst_pluginmanager good_plugin dup_plugin wrongiid_plugin)
add_test(NAME tst_pluginmanager COMMAND tst_pluginmanager)

# Step 3: datascope_add_plugin(). The test executable plays the app:
# the helper must copy the demo plugin next to it, where
# PluginManager::defaultPluginDir() finds it, with its QML embedded.
qt_add_executable(tst_pluginhelper tst_pluginhelper.cpp)
target_link_libraries(tst_pluginhelper PRIVATE pluginhost Qt6::Test)
add_test(NAME tst_pluginhelper COMMAND tst_pluginhelper)
add_subdirectory(demoplugin)
