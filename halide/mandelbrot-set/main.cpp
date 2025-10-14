#include <Halide.h>
#include <OpenImageIO/imageio.h>
#include <spdlog/spdlog.h>

#include <argparse/argparse.hpp>
#include <vector>

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("mandelbrot");
  program.add_argument("range")
      .help("plot range with xmin, xmax, ymin, ymax")
      .nargs(4)
      .default_value(std::vector<float>{-1, 1, -1, 1})
      .scan<'g', float>();
  program.add_argument("output")
      .help("output image path")
      .default_value(std::string("resources/output.png"));
  program.add_argument("-r", "--resolution")
      .help("render image resolution")
      .nargs(2)
      .default_value(std::vector<int>{1024, 1024})
      .scan<'i', int>();

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception &err) {
    spdlog::error("Threw {} when parsing arguments.", err.what());
    return -1;
  }

  std::vector<float> range = program.get<std::vector<float>>("range");
  float xMin = range[0];
  float xMax = range[1];
  float yMin = range[2];
  float yMax = range[3];

  std::vector<int> resolution = program.get<std::vector<int>>("--resolution");
  int w = resolution[0];
  int h = resolution[1];

  Halide::Var x, y, c;

  // Complex coordinate.
  Halide::Func coord;
  coord(x, y) = Halide::Tuple(
      ((Halide::cast<float>(x) + 0.5f) / float(w)) * (xMax - xMin) + xMin,
      ((Halide::cast<float>(y) + 0.5f) / float(h)) * (yMax - yMin) + yMin);

  Halide::Func mandelbrot;
  Halide::Var t;
  mandelbrot(x, y, t) = Halide::Tuple(0.0f, 0.0f);
  Halide::RDom r(1, 16);
  mandelbrot(x, y, r) = Halide::Tuple(
      mandelbrot(x, y, r - 1)[0] * mandelbrot(x, y, r - 1)[0] -
          mandelbrot(x, y, r - 1)[1] * mandelbrot(x, y, r - 1)[1] +
          coord(x, y)[0],
      2 * mandelbrot(x, y, r - 1)[0] * mandelbrot(x, y, r - 1)[1] +
          coord(x, y)[1]);

  Halide::Expr escape =
      (pow(mandelbrot(x, y, r)[0], 2) + pow(mandelbrot(x, y, r)[1], 2)) < 8;

  Halide::Func out;
  out(c, x, y) = Halide::cast<uint8_t>(Halide::argmin(escape)[0] * 255 / 16);

  // Realize the function.
  Halide::Buffer<uint8_t> output;
  try {
    output = out.realize({3, w, h});
  } catch (const Halide::Error &err) {
    spdlog::error("Threw {} when realizing Halide function.", err.what());
    return -1;
  }
  spdlog::info("Processed image.");

  // Save image.
  std::string outputPath = program.get<std::string>("output");
  auto imageOutput = OIIO::ImageOutput::create(outputPath);
  if (!imageOutput) {
    spdlog::error("Create output image failed!");
    return -1;
  }
  OIIO::ImageSpec outSpec(w, h, 3, OIIO::TypeDesc::UINT8);
  if (!imageOutput->open(outputPath, outSpec)) {
    spdlog::error("Open output image failed!");
    return -1;
  }
  if (!imageOutput->write_image(OIIO::TypeDesc::UINT8, output.data())) {
    spdlog::error("Write output image failed!");
    return -1;
  }
  spdlog::info("Wrote output image to {}.", outputPath);
  imageOutput->close();

  return 0;
}
