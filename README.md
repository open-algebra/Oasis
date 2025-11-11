![OASIS Big Cat Logo](docs/Big%20Cat%20Large.png)
<small>Credit: Mansi Panwar for artwork</small>

# OASIS: Open Algebra Software for Inferring Solutions

OASIS is an **open-source**, cross-platform C++23 library for symbolic computation in computer algebra. It runs **entirely offline** and automatically sets up all its dependencies, providing a seamless experience for developers. OASIS empowers users to perform symbolic computations with ease and flexibility—ideal for building scientific applications.

---

## What is Computer Algebra?

Computer algebra focuses on **symbolic computation**—manipulating mathematical expressions symbolically rather than approximating their numerical solutions. Unlike **numerical methods**, which produce approximate numeric results (e.g., approximating π as 3.14159), computer algebra returns exact symbolic results (e.g., leaving π as π). This makes computer algebra especially valuable in domains requiring high precision, like theoretical physics, higher mathematics, and symbolic calculus.

---

## Why Choose OASIS?

- **Open Source**: Fully open and free for anyone to use!
- **Cross-Platform**: Works seamlessly on Windows, macOS, and Linux.
- **Offline Functionality**: No internet connection required after setup.
- **Automatic Dependency Management**: Dependencies like Boost, Eigen, and Catch2 are automatically set up with CMake.
- **C++23 Powered**: OASIS provides modern C++ functionality.

---

## Building the OASIS Library with CMake

The OASIS project uses CMake to configure, build, and manage dependencies. Below are step-by-step instructions for building the library:

### Requirements

- CMake >= 3.18
- A C++23 compatible compiler
- Git for fetching dependencies

### Build Steps

1. Clone the repository:

```shell script
git clone https://github.com/open-algebra/Oasis.git
   cd Oasis
```

2. Configure and build with CMake:

```shell script
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
   cmake --build build -j$(nproc)
```

### Available CMake Options

OASIS provides several configuration options to customize the build:

- **`OASIS_BUILD_IO`**: Build extra modules for input/output serialization (default: `OFF`).
- **`OASIS_BUILD_TESTS`**: Enable unit tests with Catch2 (default: `ON`).
- **`OASIS_BUILD_WITH_COVERAGE`**: Enable code coverage instrumentation (Clang only, default: `OFF`).
- **`OASIS_BUILD_PARANOID`**: Treat warnings as errors (default: `OFF`).
- **`OASIS_BUILD_CLI`**: Build the command-line interface for OASIS (default: `OFF`).
- **`OASIS_BUILD_JS`**: Build WebAssembly bindings with Emscripten (default: `OFF`).

For example, to enable the CLI and IO modules:

```shell script
cmake -B build -S . -DOASIS_BUILD_CLI=ON -DOASIS_BUILD_IO=ON
cmake --build build
```

---

## Building an Application with OASIS Using FetchContent

You can easily integrate OASIS into your project using CMake's `FetchContent`. Here's how you can set it up:

### Example `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.18)
project(MyApp)

include(FetchContent)

# Fetch OASIS from GitHub
FetchContent_Declare(
    Oasis
    GIT_REPOSITORY https://github.com/open-algebra/Oasis.git
    GIT_TAG main  # Replace "main" with a specific tag or commit for stability
)

# Optional: Enable I/O or other OASIS modules
# set(OASIS_BUILD_IO ON)

FetchContent_MakeAvailable(Oasis)

# Define your application
add_executable(MyApp main.cpp)

# Link OASIS library
target_link_libraries(MyApp PRIVATE Oasis::Oasis)

# Optional: Link I/O or other OASIS modules
# target_link_libraries(MyApp PRIVATE Oasis::IO)
```

### Example Code (`main.cpp`)

```c++
#include <cstdlib>
#include <print> // C++23
#include <utility>
#include <memory>

#include <Oasis/Add.hpp>
#include <Oasis/Real.hpp>
#include <Oasis/SimplifyVisitor.hpp>

int main() {
    SimplifyVisitor simplifyVisitor{};
    Oasis::Add sum {
        Oasis::Real{2.0},
        Oasis::Real{3.0}
    };

    auto expr = sum.Accept(simplifiedVisitor);
    if (!expr) { // Error Case
        std::println(expr.error());
        return EXIT_FAILURE;
    }
    auto simplified = std::move(expr).value();
    std::println("Result: {}", simplified->ToString());

    return EXIT_SUCCESS;
}
```

### Build Your Application

```shell script
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
./MyApp
```

---

## Contributing to OASIS

We welcome contributions! Whether you're fixing a bug, implementing a new feature, or improving documentation, feel free to submit a pull request. Check out the [contributing guide](https://github.com/open-algebra/Oasis) for details.

---

**Get started with OASIS today and unleash the power of computer algebra in your applications!**