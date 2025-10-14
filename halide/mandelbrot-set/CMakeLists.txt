# Main executable
add_executable(mandelbrot ./main.cpp)
target_link_libraries(mandelbrot PRIVATE spdlog::spdlog)
target_link_libraries(mandelbrot PRIVATE argparse::argparse)
target_link_libraries(mandelbrot PRIVATE OpenImageIO::OpenImageIO)
target_link_libraries(mandelbrot PRIVATE Halide::Halide)

install(TARGETS mandelbrot DESTINATION bin)
