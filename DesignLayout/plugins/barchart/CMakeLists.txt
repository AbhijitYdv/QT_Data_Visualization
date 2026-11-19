# Bar chart plugin -> barchart_plugin (.dylib/.so/.dll) in the app's plugin folder.
datascope_add_plugin(barchart_plugin
    SOURCES   barchartplugin.cpp barbackend.h barbackend.cpp
    QML_FILES BarChart.qml
    RESOURCES icon.svg
    LIBRARIES xyplot)

option(BUILD_BARCHART_TESTS "Build the bar chart plugin tests (run them with ctest)" ON)
if (BUILD_BARCHART_TESTS)
    add_subdirectory(tests)
endif()
