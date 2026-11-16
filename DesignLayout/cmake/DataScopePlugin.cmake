# datascope_add_plugin: builds one chart as a runtime plugin.
#
#   datascope_add_plugin(barchart_plugin
#       APP        DesignLayoutApp            # optional, default ${CMAKE_PROJECT_NAME}
#       SOURCES    barchartplugin.cpp barbackend.h barbackend.cpp
#       QML_FILES  BarChart.qml               # -> qrc:/plugins/barchart_plugin/BarChart.qml
#       RESOURCES  icon.svg                   # -> qrc:/plugins/barchart_plugin/icon.svg
#       LIBRARIES  chartdata Qt6::Qml)        # extra links, optional
#
# What it does:
#  - a SHARED Qt plugin (.dylib/.so/.dll) linked to pluginapi only (+ LIBRARIES);
#  - QML_FILES and RESOURCES embedded under qrc:/plugins/<target>/, so the
#    plugin ships its own view (no extra files to install);
#  - after every build, copies the plugin to where PluginManager looks:
#        macOS bundle: <App>.app/Contents/PlugIns/datascope/
#        otherwise:    <folder of the app>/plugins/
#  - building the app also builds the plugin.
#  - install() puts it in the same place inside the installed app.
#
# Call it AFTER the app target exists (add the plugin subdirectories
# after qt_add_executable).

function(datascope_add_plugin target)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "APP" "SOURCES;QML_FILES;RESOURCES;LIBRARIES")

    if (NOT arg_APP)
        set(arg_APP ${CMAKE_PROJECT_NAME})
    endif()
    if (NOT TARGET ${arg_APP})
        message(FATAL_ERROR "datascope_add_plugin(${target}): app target '${arg_APP}' "
                            "does not exist yet - add the plugin after qt_add_executable.")
    endif()
    if (NOT arg_SOURCES)
        message(FATAL_ERROR "datascope_add_plugin(${target}): SOURCES is required")
    endif()

    qt_add_plugin(${target} SHARED)
    target_sources(${target} PRIVATE ${arg_SOURCES})
    target_link_libraries(${target} PRIVATE pluginapi ${arg_LIBRARIES})

    set(files ${arg_QML_FILES} ${arg_RESOURCES})
    if (files)
        qt_add_resources(${target} "${target}_files"
            PREFIX "/plugins/${target}"
            BASE   "${CMAKE_CURRENT_SOURCE_DIR}"
            FILES  ${files})
    endif()

    # Where the app looks (must match PluginManager::defaultPluginDir()).
    get_target_property(is_bundle ${arg_APP} MACOSX_BUNDLE)
    if (APPLE AND is_bundle)
        set(dest "$<TARGET_BUNDLE_CONTENT_DIR:${arg_APP}>/PlugIns/datascope")
        set(install_dest "${arg_APP}.app/Contents/PlugIns/datascope")
    else()
        set(dest "$<TARGET_FILE_DIR:${arg_APP}>/plugins")
        set(install_dest "${CMAKE_INSTALL_BINDIR}/plugins")
        if (NOT CMAKE_INSTALL_BINDIR)
            set(install_dest "bin/plugins")
        endif()
    endif()

    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${dest}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${target}>" "${dest}/"
        COMMENT "Copying ${target} into the app's plugin folder"
        VERBATIM)

    add_dependencies(${arg_APP} ${target})

    install(TARGETS ${target}
        LIBRARY DESTINATION "${install_dest}"
        RUNTIME DESTINATION "${install_dest}")
endfunction()
