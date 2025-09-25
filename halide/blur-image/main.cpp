#include <Halide.h>
#include <OpenImageIO/imageio.h>
#include <spdlog/spdlog.h>

#include <argparse/argparse.hpp>
#include <vector>

int main(int argc, char *argv[]) {
  // CLI arguments.
  argparse::ArgumentParser program("blur_image");
  program.add_argument("image").help("blur image path");
  program.add_argument("-o", "--output")
      .default_value(std::string("output.png"))
      .help("output image path");

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception &err) {
    spdlog::error("Threw {} when parsing arguments.", err.what());
    return -1;
  }

  // Load image.
  std::string imagePath = program.get<std::string>("image");
  auto imageInput = OIIO::ImageInput::open(imagePath);
  if (!imageInput) {
    spdlog::error("Open image failed!");
    return -1;
  }
  spdlog::info("Opened image from {}.", imagePath);

  const OIIO::ImageSpec &spec = imageInput->spec();
  const int w = spec.width, h = spec.height, c = spec.nchannels;
  std::vector<uint8_t> pixels(w * h * c);
  if (!imageInput->read_image(0, 0, 0, c, OIIO::TypeDesc::UINT8,
                              pixels.data())) {
    spdlog::error("Read image failed!");
    return -1;
  }
  spdlog::info("Read image.");
  imageInput->close();

  // Cretate Halide buffer from image.
  Halide::Buffer<uint8_t> input(pixels.data(), c, w, h);

  // Function index.
  Halide::Var z, x, y;

  // Clamp index (padding).
  Halide::Expr clamped_x = Halide::clamp(x, 0, w - 1);
  Halide::Expr clamped_y = Halide::clamp(y, 0, h - 1);
  Halide::Func clamped_input;
  clamped_input(z, x, y) = input(z, clamped_x, clamped_y);

  // Cast to uint16_t
  Halide::Func input_16;
  input_16(z, x, y) = Halide::cast<uint16_t>(clamped_input(z, x, y));

  // Horizontal blur.
  Halide::Func blur_x;
  blur_x(z, x, y) =
      (input_16(z, x - 1, y) + 2 * input_16(z, x, y) + input_16(z, x + 1, y)) /
      4;
  // Vertical blur.
  Halide::Func blur_y;
  blur_y(z, x, y) =
      (blur_x(z, x, y - 1) + 2 * blur_x(z, x, y) + blur_x(z, x, y + 1)) / 4;

  // Cast back to uint8_t.
  Halide::Func out_8;
  out_8(z, x, y) = Halide::cast<uint8_t>(blur_y(z, x, y));

  // Realize the function.
  Halide::Buffer<uint8_t> output;
  try {
    output = out_8.realize({c, w, h});
  } catch (const Halide::Error &err) {
    spdlog::error("Threw {} when realizing Halide function.", err.what());
    return -1;
  }
  spdlog::info("Processed image.");

  // Save image.
  std::string outputPath = program.get<std::string>("--output");
  auto imageOutput = OIIO::ImageOutput::create(outputPath);
  if (!imageOutput) {
    spdlog::error("Create output image failed!");
    return -1;
  }
  OIIO::ImageSpec outSpec(w, h, c, OIIO::TypeDesc::UINT8);
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
