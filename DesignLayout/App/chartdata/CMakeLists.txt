# Calculations and the table model behind the new dashboard cards.
# Needs only Qt Core (plus the parsers for number/date parsing), so the
# tests run without any QML or Charts module.

find_package(Qt6 REQUIRED COMPONENTS Core Sql) # Sql: used by dataparsers

qt_add_library(chartdata STATIC
    chartdata.h chartdata.cpp
    datasettablemodel.h datasettablemodel.cpp
)

target_include_directories(chartdata PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(chartdata PUBLIC cxx_std_17)
target_link_libraries(chartdata PUBLIC Qt6::Core dataparsers)

option(BUILD_CHARTDATA_TESTS "Build the chart data tests (run them with ctest)" ON)
if (BUILD_CHARTDATA_TESTS)
    add_subdirectory(tests)
endif()
