FetchContent_Declare(
        Boost
        URL "https://github.com/boostorg/boost/releases/download/boost-1.87.0/boost-1.87.0-cmake.tar.xz"
        URL_HASH SHA256=7da75f171837577a52bbf217e17f8ea576c7c246e4594d617bfde7fafd408be5
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        EXCLUDE_FROM_ALL
        FIND_PACKAGE_ARGS CONFIG
)

set(BOOST_INCLUDE_LIBRARIES any callable_traits mpl)
set(BOOST_ENABLE_COMPATIBILITY_TARGETS ON)
FetchContent_MakeAvailable(Boost)