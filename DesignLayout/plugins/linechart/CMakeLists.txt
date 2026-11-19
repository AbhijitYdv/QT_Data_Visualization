# Line chart plugin -> linechart_plugin (.dylib/.so/.dll) in the app's plugin folder.
datascope_add_plugin(linechart_plugin
    SOURCES   linechartplugin.cpp
    QML_FILES LineChart.qml
    RESOURCES icon.svg
    LIBRARIES xyplot)
