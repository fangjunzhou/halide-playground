# Main executable
add_executable(process_image ./main.cpp)
target_link_libraries(process_image PRIVATE spdlog::spdlog)
target_link_libraries(process_image PRIVATE argparse::argparse)
target_link_libraries(process_image PRIVATE OpenImageIO::OpenImageIO)
target_link_libraries(process_image PRIVATE Halide::Halide)

install(TARGETS process_image DESTINATION bin)
