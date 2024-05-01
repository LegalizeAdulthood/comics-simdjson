include(GoogleTest)

find_package(GTest CONFIG REQUIRED)

add_executable(test-comics-simdjson test.cpp)
target_link_libraries(test-comics-simdjson comics GTest::gtest_main)

gtest_discover_tests(test-comics-simdjson)
