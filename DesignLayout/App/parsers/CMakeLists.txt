# Data file parsers: a small static library used by the app and by the
# parser tests. It needs only Qt Core and Qt Sql (no QML or Charts), so the
# tests build and run quickly on their own.

find_package(Qt6 REQUIRED COMPONENTS Core Sql)

qt_add_library(dataparsers STATIC
    ../graphtable.h
    dataparser.h dataparser.cpp
    formatparsers.h
    parserutils.h parserutils.cpp
    delimitedparser.cpp
    jsonparser.cpp
    geojsonparser.cpp
    sqliteparser.cpp
    xmlparser.cpp
)

target_include_directories(dataparsers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}      # dataparser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/..   # graphtable.h
)
target_compile_features(dataparsers PUBLIC cxx_std_17)
target_link_libraries(dataparsers
    PUBLIC Qt6::Core
    PRIVATE Qt6::Sql
)

option(BUILD_PARSER_TESTS "Build the data parser tests (run them with ctest)" ON)
if (BUILD_PARSER_TESTS)
    add_subdirectory(tests)
endif()
