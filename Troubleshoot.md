1. Problems -> Unable to resolve configuration with compilerPath: "\\wsl.localhost\Ubuntu\usr\bin\g++"
    If you work with WSL, click on top middle search bar -> Show and Run Commands -> WSL - Reopen folder in WSL

2. Unable to run CTests
    Make sure you are in the build folder, and then run the command "ctest"

3. Updating GCC to proper version for WSL - Follow the steps below in your terminal 
    Source: https://stackoverflow.com/questions/62215963/how-to-install-gcc-and-gdb-for-wslwindows-subsytem-for-linux

    a. Update and upgrade: sudo apt-get update && sudo apt-get upgrade -y
    b. Clean unrequired packages sudo apt autoremove -y
    c. Install GCC: sudo apt-get install gcc -y
    d. Check version: gcc --version

4. Installing Clang 17 in Ubuntu for version 22.04
    Source: https://ubuntuhandbook.org/index.php/2023/09/how-to-install-clang-17-or-16-in-ubuntu-22-04-20-04/

    a. Download installation script: wget https://apt.llvm.org/llvm.sh
    b. Add executable permissions: chmod u+x llvm.sh
    c. Install Clang-17: sudo ./llvm.sh 17
    d. Check version: clang-xx --version

    


    