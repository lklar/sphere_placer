# CMakeList.txt : CMake project for sphere_placer, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (sphere_placer "main.cpp" "main.h" "Placer.cpp" "Placer.h" "file_interface.h" "PlacerConfig.cpp" "PlacerConfig.h" "RangeTracker.cpp" "RangeTracker.h" "Raster.cpp" "Raster.h" "vector_manipulation.cpp" "vector_manipulation.h")

add_library(RNG "RNG/Splitmix64.cpp" "RNG/xoshiro256pp.cpp" "RNG/xoshiro256ss.cpp" "RNG/RNG.cpp")

target_link_libraries(sphere_placer PUBLIC RNG)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET sphere_placer PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
