FetchContent_Declare(
        Boost
        URL "https://github.com/boostorg/boost/releases/download/boost-1.87.0/boost-1.87.0-cmake.zip"
        URL_HASH SHA256=03530dec778bc1b85b070f0b077f3b01fd417133509bb19fe7c142e47777a87b
        DOWNLOAD_EXTRACT_TIMESTAMP ON
        EXCLUDE_FROM_ALL
)

set(BOOST_INCLUDE_LIBRARIES any callable_traits mpl)
FetchContent_MakeAvailable(Boost)