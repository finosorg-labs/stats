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

# Set compile flags for SIMD variants
# The files contain multiple SIMD implementations with function-level target attributes
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(FC_SIMD_FLAGS "-msse4.2 -mavx2 -mavx512f")
elseif(MSVC)
    set(FC_SIMD_FLAGS "/arch:AVX512")
endif()

if(FC_SIMD_FLAGS)
    set_source_files_properties(mean_var.c PROPERTIES COMPILE_FLAGS "${FC_SIMD_FLAGS}")
    set_source_files_properties(kahan_sum.c PROPERTIES COMPILE_FLAGS "${FC_SIMD_FLAGS}")
endif()
