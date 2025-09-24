# Main executable
add_executable(getting_started ./main.cpp)
target_link_libraries(getting_started PRIVATE spdlog::spdlog)
target_link_libraries(getting_started PRIVATE Halide::Halide)

install(TARGETS getting_started DESTINATION bin)
