# ##############################################################################
# OASIS: Open Algebra Software for Inferring Solutions
#
# CMakeLists.txt - Eigen
# ##############################################################################

# See https://stackoverflow.com/a/65899078.

# Fetches the "Eigen" dependency and integrates it into the project.
FetchContent_Declare(
    Eigen
    GIT_REPOSITORY      https://gitlab.com/libeigen/eigen.git
    GIT_TAG             5.0.1
    EXCLUDE_FROM_ALL
    FIND_PACKAGE_ARGS   CONFIG)

set(EIGEN_BUILD_PKGCONFIG OFF)
set(EIGEN_BUILD_DOC OFF)
set(BUILD_TESTING OFF)

FetchContent_MakeAvailable(Eigen)
