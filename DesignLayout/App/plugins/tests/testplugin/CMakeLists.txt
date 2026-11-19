# Three tiny plugins built as MODULE libraries, like the real chart plugins.

function(fixture_plugin target)
    qt_add_plugin(${target} SHARED)
    target_sources(${target} PRIVATE ${ARGN}) # works on every Qt 6.x
    target_link_libraries(${target} PRIVATE pluginapi)
    set_target_properties(${target} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${FIXTURE_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${FIXTURE_DIR}) # Windows puts .dll here
endfunction()

fixture_plugin(good_plugin goodplugin.cpp)
fixture_plugin(dup_plugin dupplugin.cpp)       # same id as good_plugin
fixture_plugin(wrongiid_plugin wrongiidplugin.cpp)
