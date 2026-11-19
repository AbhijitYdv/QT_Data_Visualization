# US map plugin -> usmap_plugin (.dylib/.so/.dll) in the app's plugin folder.
# usmapview.* is the QPainter map (was App/usmap/usmapitem), registered for
# QML by the plugin; usmap (App/usmap) still provides projection + outlines.
datascope_add_plugin(usmap_plugin
    SOURCES   usmapplugin.cpp mapbackend.h mapbackend.cpp usmapview.h usmapview.cpp
    QML_FILES UsMap.qml ColumnPicker.qml
    RESOURCES icon.svg
    LIBRARIES usmap modelaccess Qt6::Quick Qt6::Qml)
