cmake_minimum_required(VERSION 3.23)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(comics-simdjson CXX)

include(CTest)

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED TRUE )
set( CMAKE_CXX_EXTENSIONS OFF )

add_executable(print-comics main.cpp)
target_link_libraries(print-comics PUBLIC comics)

add_subdirectory(comics)
add_subdirectory(tools)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
