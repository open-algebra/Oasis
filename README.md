# OASIS ![GitHub Workflow Status (with event)](https://img.shields.io/github/actions/workflow/status/matthew-mccall/oasis/cmake-multi-platform.yml)

Open Algebra Software for Inferring Solutions is a C++ library for embedding computer algebra and symbolic manipulation.

## Building
The library is written in C++20 and is built with CMake. All dependencies are managed by CMake itself in an effort to be highly portable. As, such all you need is:
* CMake - The build tool
* Git - Used by CMake to automatically download dependencies
* A modern C++ compiler (See below)

### Supported Compilers

* AppleClang ≥ 15
* Clang ≥ 15 (≥ 16 for Ubuntu)
* MSVC
* GCC ≥ 13.2.1

#### Unsupported Tools
* NMake seems to have problems. We recommend [Ninja](https://ninja-build.org) or Makefiles.

### Example Build Commands

Configure (Be sure to replace `<your-preferred-build-folder>` and `<your-preferred-build-tool>` with your preferred
build folder and build tool respectively):

```shell
cmake -B <your-preferred-build-folder> -G <your-preferred-build-tool> .
```

Build:

```shell
cmake --build <your-preferred-build-folder>
```
