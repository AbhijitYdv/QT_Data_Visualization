# Area chart plugin -> areachart_plugin (.dylib/.so/.dll) in the app's plugin folder.
datascope_add_plugin(areachart_plugin
    SOURCES   areachartplugin.cpp
    QML_FILES AreaChart.qml
    RESOURCES icon.svg
    LIBRARIES xyplot)
