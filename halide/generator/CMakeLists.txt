# Main executable
add_halide_generator(my_generators SOURCES generator.cpp)
add_halide_library(my_first_generator FROM my_generators)
