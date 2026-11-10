find_package(Qt6 REQUIRED COMPONENTS Test)

qt_add_executable(tst_chartdata tst_chartdata.cpp)
target_link_libraries(tst_chartdata PRIVATE chartdata Qt6::Test)

add_test(NAME chartdata COMMAND tst_chartdata)
