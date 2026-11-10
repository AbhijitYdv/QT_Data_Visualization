find_package(Qt6 REQUIRED COMPONENTS Sql Test)

qt_add_executable(tst_parsers tst_parsers.cpp)
target_link_libraries(tst_parsers PRIVATE dataparsers Qt6::Sql Qt6::Test)

# The sample files in the repo's samples/ folder double as test data.
target_compile_definitions(tst_parsers PRIVATE
    SAMPLES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../../../../samples"
)

add_test(NAME parsers COMMAND tst_parsers)
