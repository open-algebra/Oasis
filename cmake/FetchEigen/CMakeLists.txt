############################################################
# OASIS: Open Algebra Software for Inferring Solutions
#
# CMakeLists.txt - Eigen
############################################################

# See https://stackoverflow.com/a/65899078.

# Fetches the "eigen" dependency and integrates it into the project.
FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
)

# Disables some of eigen's build options.
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(EIGEN_BUILD_PKGCONFIG OFF)
set(EIGEN_BUILD_DOC OFF)
set(BUILD_TESTING OFF)

FetchContent_MakeAvailable(eigen)
