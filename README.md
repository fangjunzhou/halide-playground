# Halide Playground

Simple examples for learning [Halide](https://halide-lang.org/).

## Setup

### Option 1: Using Nix (Recommended)
```bash
nix develop
uv venv
source .venv/bin/activate
uv pip install -e .
```

### Option 2: Manual Installation
Install dependencies:
- CMake 3.31+
- C++17 compiler
- Halide library
- spdlog, argparse, OpenImageIO
- Python 3.12+

Then setup Python environment:
```bash
python3.12 -m venv .venv
source .venv/bin/activate
pip install -e .
```

## Build and Run

### C++ Examples

Build:
```bash
cmake -S . -B build
make -C build -j$(nproc)
```

Run examples:
```bash
# Basic gradient example
./bin/getting_started

# Image processing (brightens image by 1.5x)
./bin/process_image path/to/image.jpg -o output.png
```
