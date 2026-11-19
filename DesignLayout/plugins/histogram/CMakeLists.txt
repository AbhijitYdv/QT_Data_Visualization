# Histogram plugin -> histogram_plugin (.dylib/.so/.dll) in the app's plugin folder.
datascope_add_plugin(histogram_plugin
    SOURCES   histogramplugin.cpp histogrambackend.h histogrambackend.cpp
    QML_FILES Histogram.qml
    RESOURCES icon.svg
    LIBRARIES chartdata modelaccess Qt6::Charts)
