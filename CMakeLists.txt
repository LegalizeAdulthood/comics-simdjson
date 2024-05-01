cmake_minimum_required(VERSION 3.23)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(comics-simdjson CXX)

include(CTest)

add_executable(print-comics main.cpp)
set_target_properties(print-comics PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON)
target_link_libraries(print-comics PUBLIC comics)

add_subdirectory(comics)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
