#include <Halide.h>
#include <spdlog/spdlog.h>

int main(int argc, char *argv[]) {
  // Halide gradient function.
  Halide::Func gradient;
  Halide::Var x, y;
  gradient(x, y) = x + y;

  // Generate Halide code.
  gradient.compile_to_lowered_stmt("resources/codegen.html", {}, Halide::HTML);
  spdlog::info("Generated Halide code to resources/codegen.html.");

  // Print nested loop structure.
  gradient.print_loop_nest();

  return 0;
}
