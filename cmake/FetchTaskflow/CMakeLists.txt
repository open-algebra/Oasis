# Fetches the "taskflow" dependency and integrates it into Oasis.
# Disables some of taskflow's build options.
FetchContent_Declare(
        taskflow
        GIT_REPOSITORY https://github.com/taskflow/taskflow.git
        GIT_TAG v3.6.0
)

set(TF_BUILD_TESTS OFF)
set(TF_BUILD_EXAMPLES OFF)

# Excludes taskflow's targets from all ALL target.
FetchContent_GetProperties(taskflow)
if (NOT taskflow_POPULATED)
    FetchContent_Populate(taskflow)
    add_subdirectory(${taskflow_SOURCE_DIR} ${taskflow_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()