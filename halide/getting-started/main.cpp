#include <Halide.h>
#include <spdlog/spdlog.h>

int main(int argc, char *argv[]) {
  // Halide gradient function.
  Halide::Func gradient;
  Halide::Var x, y;
  gradient(x, y) = x + y;

  // Realize the function.
  Halide::Buffer<int32_t> output = gradient.realize({800, 600});

  // Check the result.
  for (int j = 0; j < output.height(); j++) {
    for (int i = 0; i < output.width(); i++) {
      if (output(i, j) != i + j) {
        spdlog::error("Output ({}, {}) mismatch.", i, j);
        return -1;
      }
    }
  }

  spdlog::info("Success!");
  return 0;
}
