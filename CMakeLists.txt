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

# Determine platform-specific build directory
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(PLATFORM_BUILD_DIR "linux_amd64")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
        set(PLATFORM_BUILD_DIR "darwin_arm64")
    else()
        set(PLATFORM_BUILD_DIR "darwin_amd64")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(PLATFORM_BUILD_DIR "windows_amd64")
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

# Link prebuilt libraries from modules/ directory
set(PLATFORM_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules/platform/build/${PLATFORM_BUILD_DIR}/libfinkit_platform_static.a")
set(MATRIX_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules/matrix/build/${PLATFORM_BUILD_DIR}/libfinkit_matrix_static.a")
set(MPFR_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules/platform/build/${PLATFORM_BUILD_DIR}/third_party/mpfr/libmpfr.a")
set(GMP_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules/platform/build/${PLATFORM_BUILD_DIR}/third_party/gmp/libgmp.a")

if(NOT EXISTS "${PLATFORM_LIB_PATH}")
    message(FATAL_ERROR "Platform library not found at: ${PLATFORM_LIB_PATH}")
endif()
if(NOT EXISTS "${MATRIX_LIB_PATH}")
    message(FATAL_ERROR "Matrix library not found at: ${MATRIX_LIB_PATH}")
endif()
if(NOT EXISTS "${MPFR_LIB_PATH}")
    message(FATAL_ERROR "MPFR library not found at: ${MPFR_LIB_PATH}")
endif()
if(NOT EXISTS "${GMP_LIB_PATH}")
    message(FATAL_ERROR "GMP library not found at: ${GMP_LIB_PATH}")
endif()

message(STATUS "Found platform library: ${PLATFORM_LIB_PATH}")
message(STATUS "Found matrix library: ${MATRIX_LIB_PATH}")
message(STATUS "Found MPFR library: ${MPFR_LIB_PATH}")
message(STATUS "Found GMP library: ${GMP_LIB_PATH}")

# Create IMPORTED targets for prebuilt libraries
add_library(finkit_platform_imported STATIC IMPORTED)
set_target_properties(finkit_platform_imported PROPERTIES
    IMPORTED_LOCATION "${PLATFORM_LIB_PATH}"
)

add_library(finkit_matrix_imported STATIC IMPORTED)
set_target_properties(finkit_matrix_imported PROPERTIES
    IMPORTED_LOCATION "${MATRIX_LIB_PATH}"
)

add_library(mpfr_imported STATIC IMPORTED)
set_target_properties(mpfr_imported PROPERTIES
    IMPORTED_LOCATION "${MPFR_LIB_PATH}"
)

add_library(gmp_imported STATIC IMPORTED)
set_target_properties(gmp_imported PROPERTIES
    IMPORTED_LOCATION "${GMP_LIB_PATH}"
)

# Stats module sources
set(STATS_SOURCES
    stats-c/mean_var.c
    stats-c/kahan_sum.c
    stats-c/welford.c
    stats-c/covariance.c
    stats-c/correlation.c
    stats-c/quantile.c
    stats-c/rolling.c
    stats-c/rank.c
    stats-c/ema.c
    stats-c/moments.c
    stats-c/zscore.c
)

# Create object library
add_library(finkit_stats OBJECT ${STATS_SOURCES})

target_include_directories(finkit_stats
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/modules/platform/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/modules/matrix/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_definitions(finkit_stats PRIVATE FC_STATIC_BUILD)

# Set compile flags for SIMD variants based on detected level
if(FC_SIMD_LEVEL STREQUAL "AVX512")
    set_source_files_properties(stats-c/mean_var.c PROPERTIES COMPILE_FLAGS "${FC_AVX512_FLAGS}")
    set_source_files_properties(stats-c/kahan_sum.c PROPERTIES COMPILE_FLAGS "${FC_AVX512_FLAGS}")
    set_source_files_properties(stats-c/welford.c PROPERTIES COMPILE_FLAGS "${FC_AVX512_FLAGS}")
    set_source_files_properties(stats-c/covariance.c PROPERTIES COMPILE_FLAGS "${FC_AVX512_FLAGS}")
    set_source_files_properties(stats-c/correlation.c PROPERTIES COMPILE_FLAGS "${FC_AVX512_FLAGS}")
    set_source_files_properties(stats-c/quantile.c PROPERTIES COMPILE_FLAGS "${FC_AVX512_FLAGS}")
    set_source_files_properties(stats-c/rolling.c PROPERTIES COMPILE_FLAGS "${FC_AVX512_FLAGS}")
    set_source_files_properties(stats-c/rank.c PROPERTIES COMPILE_FLAGS "${FC_AVX512_FLAGS}")
    set_source_files_properties(stats-c/ema.c PROPERTIES COMPILE_FLAGS "${FC_AVX512_FLAGS}")
    set_source_files_properties(stats-c/moments.c PROPERTIES COMPILE_FLAGS "${FC_AVX512_FLAGS}")
    set_source_files_properties(stats-c/zscore.c PROPERTIES COMPILE_FLAGS "${FC_AVX512_FLAGS}")
elseif(FC_SIMD_LEVEL STREQUAL "AVX2")
    set_source_files_properties(stats-c/mean_var.c PROPERTIES COMPILE_FLAGS "${FC_AVX2_FLAGS}")
    set_source_files_properties(stats-c/kahan_sum.c PROPERTIES COMPILE_FLAGS "${FC_AVX2_FLAGS}")
    set_source_files_properties(stats-c/welford.c PROPERTIES COMPILE_FLAGS "${FC_AVX2_FLAGS}")
    set_source_files_properties(stats-c/covariance.c PROPERTIES COMPILE_FLAGS "${FC_AVX2_FLAGS}")
    set_source_files_properties(stats-c/correlation.c PROPERTIES COMPILE_FLAGS "${FC_AVX2_FLAGS}")
    set_source_files_properties(stats-c/quantile.c PROPERTIES COMPILE_FLAGS "${FC_AVX2_FLAGS}")
    set_source_files_properties(stats-c/rolling.c PROPERTIES COMPILE_FLAGS "${FC_AVX2_FLAGS}")
    set_source_files_properties(stats-c/rank.c PROPERTIES COMPILE_FLAGS "${FC_AVX2_FLAGS}")
    set_source_files_properties(stats-c/ema.c PROPERTIES COMPILE_FLAGS "${FC_AVX2_FLAGS}")
    set_source_files_properties(stats-c/moments.c PROPERTIES COMPILE_FLAGS "${FC_AVX2_FLAGS}")
    set_source_files_properties(stats-c/zscore.c PROPERTIES COMPILE_FLAGS "${FC_AVX2_FLAGS}")
elseif(FC_SIMD_LEVEL STREQUAL "SSE42")
    set_source_files_properties(stats-c/mean_var.c PROPERTIES COMPILE_FLAGS "${FC_SSE42_FLAGS}")
    set_source_files_properties(stats-c/kahan_sum.c PROPERTIES COMPILE_FLAGS "${FC_SSE42_FLAGS}")
    set_source_files_properties(stats-c/welford.c PROPERTIES COMPILE_FLAGS "${FC_SSE42_FLAGS}")
    set_source_files_properties(stats-c/covariance.c PROPERTIES COMPILE_FLAGS "${FC_SSE42_FLAGS}")
    set_source_files_properties(stats-c/correlation.c PROPERTIES COMPILE_FLAGS "${FC_SSE42_FLAGS}")
    set_source_files_properties(stats-c/quantile.c PROPERTIES COMPILE_FLAGS "${FC_SSE42_FLAGS}")
    set_source_files_properties(stats-c/rolling.c PROPERTIES COMPILE_FLAGS "${FC_SSE42_FLAGS}")
    set_source_files_properties(stats-c/rank.c PROPERTIES COMPILE_FLAGS "${FC_SSE42_FLAGS}")
    set_source_files_properties(stats-c/ema.c PROPERTIES COMPILE_FLAGS "${FC_SSE42_FLAGS}")
    set_source_files_properties(stats-c/moments.c PROPERTIES COMPILE_FLAGS "${FC_SSE42_FLAGS}")
    set_source_files_properties(stats-c/zscore.c PROPERTIES COMPILE_FLAGS "${FC_SSE42_FLAGS}")
endif()

# Create static library
add_library(finkit_stats_static STATIC $<TARGET_OBJECTS:finkit_stats>)
target_include_directories(finkit_stats_static
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Link prebuilt libraries
# Use --start-group/--end-group to handle circular dependencies in static linking
target_link_libraries(finkit_stats_static PUBLIC
    -Wl,--start-group
    finkit_matrix_imported
    finkit_platform_imported
    mpfr_imported
    gmp_imported
    -Wl,--end-group
    m
    gcov
)

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
