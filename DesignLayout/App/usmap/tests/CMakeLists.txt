find_package(Qt6 REQUIRED COMPONENTS Test)

qt_add_executable(tst_usmap tst_usmap.cpp)
target_link_libraries(tst_usmap PRIVATE usmap Qt6::Test)

target_compile_definitions(tst_usmap PRIVATE
    SAMPLES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../../../../samples"
)

add_test(NAME usmap COMMAND tst_usmap)
