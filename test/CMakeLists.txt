add_executable(test_core test_core.cpp)
target_include_directories(test_core PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_core PRIVATE core_lib)
add_test(NAME test_core COMMAND test_core)
