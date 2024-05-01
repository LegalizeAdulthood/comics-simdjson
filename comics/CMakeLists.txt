find_package(simdjson CONFIG REQUIRED)

add_library(comics comics.h comics.cpp)
target_include_directories(comics PUBLIC .)
target_link_libraries(comics PUBLIC simdjson::simdjson)
