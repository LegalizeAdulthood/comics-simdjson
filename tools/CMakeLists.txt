find_package(Boost REQUIRED COMPONENTS algorithm)

add_executable(gcd-to-json gcd-to-json.cpp)
set_target_properties(gcd-to-json PROPERTIES FOLDER "Tools")
target_link_libraries(gcd-to-json PUBLIC Boost::algorithm)
