# Main executable
add_executable(stmt_codegen ./main.cpp)
target_link_libraries(stmt_codegen PRIVATE spdlog::spdlog)
target_link_libraries(stmt_codegen PRIVATE Halide::Halide)

install(TARGETS stmt_codegen DESTINATION bin)
