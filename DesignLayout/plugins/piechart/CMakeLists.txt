# Pie chart plugin -> piechart_plugin (.dylib/.so/.dll) in the app's plugin folder.
datascope_add_plugin(piechart_plugin
    SOURCES   piechartplugin.cpp piebackend.h piebackend.cpp
    QML_FILES PieChart.qml ColumnPicker.qml
    RESOURCES icon.svg
    LIBRARIES chartdata modelaccess)
