include_directories(.)
set(SOURCES main.cpp Placer.cpp PlacerConfig.cpp RangeTracker.cpp Raster.cpp vector_manipulation.cpp)
add_executable(sphere_placer ${SOURCES})
target_link_libraries(sphere_placer PUBLIC RNG)
target_include_directories(
   sphere_placer PUBLIC
   "${PROJECT_BINARY_DIR}"
   "${PROJECT_BINARY_DIR}/include"
   "${PROJECT_SOURCE_DIR}/src/RNG"
   "${PROJECT_SOURCE_DIR}/src/dim_N_vectors"
   "${PROJECT_SOURCE_DIR}/src/file_interface"
)
set_property(TARGET sphere_placer PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})