FetchContent_Declare(
        tinyxml2
        GIT_REPOSITORY https://github.com/leethomason/tinyxml2
        GIT_TAG 10.0.0)

set(tinyxml2_BUILD_TESTING OFF)

FetchContent_MakeAvailable(tinyxml2)