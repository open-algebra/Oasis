# OASIS ![GitHub Workflow Status (with event)](https://img.shields.io/github/actions/workflow/status/matthew-mccall/oasis/cmake-multi-platform.yml)

Open Algebra Software for Inferring Solutions is a C++ for embedding computer algebra and symbolic manipulation.

## Building
The library is written in C++20 and is built with CMake. All dependencies are managed by CMake itself in an effort to be highly portable. As, such all you need is CMake and a modern C++ compiler. 

### Supported Compilers

* AppleClang ≥ 15
* Clang ≥ 15 (≥ 16 for Ubuntu)
* MSVC

#### Unsupported Tools
* There is [a bug in GCC](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=111485) that prevents the library from compiling,
a fix is in the master branch of GCC but has not been released yet.
  * Nautrally, MinGW does not work. If you're using CLion on Windows, you'll need to use the Visual Studio toolchain in CLion.
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
