# Scatter plot plugin -> scatterplot_plugin (.dylib/.so/.dll) in the app's plugin folder.
datascope_add_plugin(scatterplot_plugin
    SOURCES   scatterplotplugin.cpp
    QML_FILES ScatterPlot.qml
    RESOURCES icon.svg
    LIBRARIES xyplot)
