FetchContent_Declare(
        Linenoise
        GIT_REPOSITORY https://github.com/antirez/linenoise.git
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
)

FetchContent_MakeAvailable(Linenoise)

add_library(Linenoise ${linenoise_SOURCE_DIR}/linenoise.c)
target_include_directories(Linenoise PUBLIC ${linenoise_SOURCE_DIR})