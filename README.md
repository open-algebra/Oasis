# OASIS
Open Algebra Software for Inferring Solutions is a C++ for embedding computer algebra and symbolic manipulation.

## Building
The library is written in C++20 and is built with CMake. All dependencies are managed by CMake itself in an effort to be highly portable. As, such all you need is CMake and a modern C++ compiler. 

If you are on macOS, OASIS currently does not build with Apple Clang. As such, you make need to install GCC or LLVM Clang from Homebrew or MacPorts.

Nevertheless, configuration and building is standard:

`cmake -B <your-preferred-build-folder> -G <your-preferred-build-tool> .`

`cmake --build <your-preferred-build-folder>`
