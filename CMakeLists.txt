cmake_minimum_required(VERSION 3.16)
project(finkit-stats VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include SIMD detection and compiler flags
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/DetectSIMD.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompilerFlags.cmake)

# Sanitizer configuration
if(FC_ENABLE_SANITIZERS)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sanitizers.cmake)
        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/sanitizers.cmake)
    endif()
endif()

# Generate cgo_source.go with detected SIMD flags
if(FC_SIMD_LEVEL STREQUAL "AVX512")
    set(FC_SIMD_CFLAGS "${FC_AVX512_FLAGS}")
elseif(FC_SIMD_LEVEL STREQUAL "AVX2")
    set(FC_SIMD_CFLAGS "${FC_AVX2_FLAGS}")
elseif(FC_SIMD_LEVEL STREQUAL "SSE42")
    set(FC_SIMD_CFLAGS "${FC_SSE42_FLAGS}")
else()
    set(FC_SIMD_CFLAGS "")
endif()
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cgo_source.go.in
    ${CMAKE_CURRENT_SOURCE_DIR}/cgo_source.go
    @ONLY
)

# Add platform submodule
add_subdirectory(modules/platform EXCLUDE_FROM_ALL)

# Stats module sources
set(STATS_SOURCES
    stats-c/mean_var.c
    stats-c/kahan_sum.c
    stats-c/welford.c
    stats-c/covariance.c
    stats-c/quantile.c
)

# Create object library
add_library(finkit_stats OBJECT ${STATS_SOURCES})

target_include_directories(finkit_stats
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/modules/platform/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_definitions(finkit_stats PRIVATE FC_STATIC_BUILD)

# Set compile flags for SIMD variants based on detected level
if(FC_SIMD_LEVEL STREQUAL "AVX512")
    set_source_files_properties(stats-c/mean_var.c PROPERTIES COMPILE_FLAGS "${FC_AVX512_FLAGS}")
    set_source_files_properties(stats-c/kahan_sum.c PROPERTIES COMPILE_FLAGS "${FC_AVX512_FLAGS}")
    set_source_files_properties(stats-c/welford.c PROPERTIES COMPILE_FLAGS "${FC_AVX512_FLAGS}")
    set_source_files_properties(stats-c/covariance.c PROPERTIES COMPILE_FLAGS "${FC_AVX512_FLAGS}")
    set_source_files_properties(stats-c/quantile.c PROPERTIES COMPILE_FLAGS "${FC_AVX512_FLAGS}")
elseif(FC_SIMD_LEVEL STREQUAL "AVX2")
    set_source_files_properties(stats-c/mean_var.c PROPERTIES COMPILE_FLAGS "${FC_AVX2_FLAGS}")
    set_source_files_properties(stats-c/kahan_sum.c PROPERTIES COMPILE_FLAGS "${FC_AVX2_FLAGS}")
    set_source_files_properties(stats-c/welford.c PROPERTIES COMPILE_FLAGS "${FC_AVX2_FLAGS}")
    set_source_files_properties(stats-c/covariance.c PROPERTIES COMPILE_FLAGS "${FC_AVX2_FLAGS}")
    set_source_files_properties(stats-c/quantile.c PROPERTIES COMPILE_FLAGS "${FC_AVX2_FLAGS}")
elseif(FC_SIMD_LEVEL STREQUAL "SSE42")
    set_source_files_properties(stats-c/mean_var.c PROPERTIES COMPILE_FLAGS "${FC_SSE42_FLAGS}")
    set_source_files_properties(stats-c/kahan_sum.c PROPERTIES COMPILE_FLAGS "${FC_SSE42_FLAGS}")
    set_source_files_properties(stats-c/welford.c PROPERTIES COMPILE_FLAGS "${FC_SSE42_FLAGS}")
    set_source_files_properties(stats-c/covariance.c PROPERTIES COMPILE_FLAGS "${FC_SSE42_FLAGS}")
    set_source_files_properties(stats-c/quantile.c PROPERTIES COMPILE_FLAGS "${FC_SSE42_FLAGS}")
endif()

# Create static library
add_library(finkit_stats_static STATIC $<TARGET_OBJECTS:finkit_stats>)
target_include_directories(finkit_stats_static
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Link platform and math library
target_link_libraries(finkit_stats_static PUBLIC finkit_platform_static m)

# Create alias
add_library(finkit::stats ALIAS finkit_stats_static)

# Install
install(TARGETS finkit_stats_static
    EXPORT finkit-stats-targets
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include/stats
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT finkit-stats-targets
    FILE finkit-statsTargets.cmake
    NAMESPACE finkit::
    DESTINATION lib/cmake/finkit-stats
)

# Config files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/finkit-statsConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/finkit-statsConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/finkit-statsConfig.cmake"
    INSTALL_DESTINATION lib/cmake/finkit-stats
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/finkit-statsConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/finkit-statsConfigVersion.cmake"
    DESTINATION lib/cmake/finkit-stats
)

# Tests
option(FC_BUILD_TESTS "Build tests" ON)
if(FC_BUILD_TESTS AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests)
    enable_testing()
    add_subdirectory(tests)
endif()

# Benchmarks
option(FC_BUILD_BENCHMARKS "Build benchmarks" ON)
if(FC_BUILD_BENCHMARKS AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks)
    add_subdirectory(benchmarks)
endif()

# Print configuration
message(STATUS "")
message(STATUS "============================================================")
message(STATUS " finkit-stats ${PROJECT_VERSION}")
message(STATUS "============================================================")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "============================================================")
message(STATUS "")
