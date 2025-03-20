add_executable(OasisCLI main.cpp)
target_link_libraries(OasisCLI PRIVATE Oasis::Oasis Oasis::IO fmt::fmt isocline)
target_compile_features(OasisCLI PRIVATE cxx_std_23)
