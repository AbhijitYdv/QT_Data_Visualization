find_package(Qt6 REQUIRED COMPONENTS Test Gui)

qt_add_executable(tst_pluginapi tst_pluginapi.cpp)
target_link_libraries(tst_pluginapi PRIVATE pluginapi Qt6::Test Qt6::Gui)
add_test(NAME tst_pluginapi COMMAND tst_pluginapi)
