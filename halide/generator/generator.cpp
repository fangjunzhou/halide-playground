#include <Halide.h>

class MyFirstGenerator : public Halide::Generator<MyFirstGenerator> {
 public:
  Input<uint8_t> offset{"offset"};
  Input<Halide::Buffer<uint8_t, 2>> input{"input"};
  Output<Halide::Buffer<uint8_t, 2>> output{"output"};

  Halide::Var x, y;

  void generate() {
    output(x, y) = Halide::max(input(x, y) + offset, 255);

    output.vectorize(x, 16).parallel(y);
  }
};

HALIDE_REGISTER_GENERATOR(MyFirstGenerator, my_first_generator)
