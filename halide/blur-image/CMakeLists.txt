# Main executable
add_executable(blur_image ./main.cpp)
target_link_libraries(blur_image PRIVATE spdlog::spdlog)
target_link_libraries(blur_image PRIVATE argparse::argparse)
target_link_libraries(blur_image PRIVATE OpenImageIO::OpenImageIO)
target_link_libraries(blur_image PRIVATE Halide::Halide)

install(TARGETS blur_image DESTINATION bin)
