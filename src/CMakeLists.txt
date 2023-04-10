add_library(RNG "RNG/Splitmix64.cpp" "RNG/xoshiro256pp.cpp" "RNG/xoshiro256ss.cpp" "RNG/RNG.cpp")
add_library(file_interface "file_interface/file_interface.cpp")

target_include_directories(
   file_interface PUBLIC
   "${PROJECT_SOURCE_DIR}/src/dim_N_vectors"
)
