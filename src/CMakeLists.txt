#
# src/stats/CMakeLists.txt - Stats module (mean and variance calculations)
#

# Stats module sources
set(STATS_SOURCES
    mean_var.c
    kahan_sum.c
)

# Create object library for stats module
add_library(stats_obj OBJECT ${STATS_SOURCES})

target_include_directories(stats_obj PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/core/platform/include
)

# Enable internal tests if building tests
if(FC_BUILD_TESTS)
    target_compile_definitions(stats_obj PRIVATE FC_ENABLE_INTERNAL_TESTS)
endif()

# Set compile flags for SIMD variants in mean_var.c
# The file contains multiple SIMD implementations with function-level target attributes
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    # Enable SSE4.2 and AVX2 for the entire file (function attributes will control actual usage)
    set_source_files_properties(mean_var.c PROPERTIES COMPILE_FLAGS "-msse4.2 -mavx2")
elseif(MSVC)
    set_source_files_properties(mean_var.c PROPERTIES COMPILE_FLAGS "/arch:AVX2")
endif()
