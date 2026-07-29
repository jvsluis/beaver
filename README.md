# Beaver
A WebGPU based game engine written in C++

## Setup
```bash
# Clone the git project
git clone https://github.com/jvsluis/beaver.git 

# Navigate to the cloned directory
cd beaver 

# Initialize the submodules with shallow depth
git submodule update --init --depth 1
```

## Building
### Platform Build
```bash
# Build the engine and example with CMake
CC=clang CXX=clang++ cmake -B build && cmake --build build -j4

# The library will be available at: build/beaver/libbeaver.a
```

### Web Build
Prior to running these commands, you must ensure that you have Emscripten installed and the Emscripten SDK environment variables set for your current shell
```bash
# Note: 

# Build the engine and example with Emscripten
emcmake cmake -B build-web && cmake --build build-web -j4

# The library will be available at: build-web/beaver/libbeaver.a
```

### Running examples
```bash
# Examples will be located in ./build/examples/
# For example:
./build/examples/basic/basic

# For web builds you'll need to run a local webserver
# For example:
python3 -m http.server 8080

# Then navigate to: http://localhost:8080/build-web/examples/basic/basic.html
```

## Usage
See the ```examples/``` directory for examples of how the engine can be used

## License
This project is licensed under the Apache License 2.0
