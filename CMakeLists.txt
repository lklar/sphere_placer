# Works with 3.14 and tested through 3.26
cmake_minimum_required(VERSION 3.14...3.26)

# Project name and a few useful settings. Other commands can pick up the results
project(sphere_placer)

# Only do these if this is the main project, and not if it is included through add_subdirectory
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# The compiled library code is here

add_subdirectory(src)
# The executable code is here
add_subdirectory(apps)
