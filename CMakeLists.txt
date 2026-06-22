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
# Remove -mfma flag for Go (not allowed in CGO)
string(REPLACE "-mfma" "" FC_SIMD_CFLAGS "${FC_SIMD_CFLAGS}")
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cgo_source.go.in
    ${CMAKE_CURRENT_SOURCE_DIR}/cgo_source.go
    @ONLY
)

# Detect platform for library paths
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
        set(STATS_BUILD_DIR "darwin_arm64")
    else()
        set(STATS_BUILD_DIR "darwin_x86_64")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(STATS_BUILD_DIR "linux_amd64")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(STATS_BUILD_DIR "windows_amd64")
endif()

# Define paths to prebuilt libraries
set(MATRIX_STATIC_LIB ${CMAKE_CURRENT_SOURCE_DIR}/modules/matrix/build/${STATS_BUILD_DIR}/libfinkit_matrix_static.a)
set(PLATFORM_STATIC_LIB ${CMAKE_CURRENT_SOURCE_DIR}/modules/platform/build/${STATS_BUILD_DIR}/libfinkit_platform_static.a)
set(SORT_STATIC_LIB ${CMAKE_CURRENT_SOURCE_DIR}/modules/sort/build/${STATS_BUILD_DIR}/libfinkit_sort_static.a)

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
    stats-c/weighted.c
    stats-c/winsorize.c
)

# Create object library
add_library(finkit_stats OBJECT ${STATS_SOURCES})

target_include_directories(finkit_stats
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/modules/platform/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/modules/matrix/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/modules/sort/include>
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
    set_source_files_properties(stats-c/weighted.c PROPERTIES COMPILE_FLAGS "${FC_AVX512_FLAGS}")
    set_source_files_properties(stats-c/winsorize.c PROPERTIES COMPILE_FLAGS "${FC_AVX512_FLAGS}")
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
    set_source_files_properties(stats-c/weighted.c PROPERTIES COMPILE_FLAGS "${FC_AVX2_FLAGS}")
    set_source_files_properties(stats-c/winsorize.c PROPERTIES COMPILE_FLAGS "${FC_AVX2_FLAGS}")
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
    set_source_files_properties(stats-c/weighted.c PROPERTIES COMPILE_FLAGS "${FC_SSE42_FLAGS}")
    set_source_files_properties(stats-c/winsorize.c PROPERTIES COMPILE_FLAGS "${FC_SSE42_FLAGS}")
endif()

# Create base static library (without dependencies)
add_library(finkit_stats_static_base STATIC $<TARGET_OBJECTS:finkit_stats>)
target_include_directories(finkit_stats_static_base
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Merge static libraries into a single library
set(MERGED_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/${STATS_BUILD_DIR}/libfinkit_stats_static.a)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Windows: use lib.exe to merge libraries
    add_custom_command(
        OUTPUT ${MERGED_LIB_PATH}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/build/${STATS_BUILD_DIR}
        COMMAND lib.exe /OUT:${MERGED_LIB_PATH}
            $<TARGET_FILE:finkit_stats_static_base>
            ${MATRIX_STATIC_LIB}
            ${SORT_STATIC_LIB}
          ${PLATFORM_STATIC_LIB}
        DEPENDS finkit_stats_static_base ${MATRIX_STATIC_LIB} ${SORT_STATIC_LIB} ${PLATFORM_STATIC_LIB}
        COMMENT "Merging static libraries into ${MERGED_LIB_PATH}"
        VERBATIM
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # macOS: use libtool to merge libraries
    add_custom_command(
        OUTPUT ${MERGED_LIB_PATH}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/build/${STATS_BUILD_DIR}
        COMMAND libtool -static -o ${MERGED_LIB_PATH}
            $<TARGET_FILE:finkit_stats_static_base>
            ${MATRIX_STATIC_LIB}
            ${SORT_STATIC_LIB}
            ${PLATFORM_STATIC_LIB}
            2>&1 | grep -v "warning duplicate member name" || true
        DEPENDS finkit_stats_static_base ${MATRIX_STATIC_LIB} ${SORT_STATIC_LIB} ${PLATFORM_STATIC_LIB}
        COMMENT "Merging static libraries into ${MERGED_LIB_PATH}"
        VERBATIM
    )
else()
    # Linux: use ar with MRI script to merge libraries
    set(MRI_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/merge_libs.mri)
    file(GENERATE OUTPUT ${MRI_SCRIPT} CONTENT
"CREATE ${MERGED_LIB_PATH}
ADDLIB $<TARGET_FILE:finkit_stats_static_base>
ADDLIB ${MATRIX_STATIC_LIB}
ADDLIB ${SORT_STATIC_LIB}
ADDLIB ${PLATFORM_STATIC_LIB}
SAVE
END
")

    add_custom_command(
        OUTPUT ${MERGED_LIB_PATH}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/build/${STATS_BUILD_DIR}
        COMMAND ${CMAKE_AR} -M < ${MRI_SCRIPT}
        DEPENDS finkit_stats_static_base ${MATRIX_STATIC_LIB} ${SORT_STATIC_LIB} ${PLATFORM_STATIC_LIB} ${MRI_SCRIPT}
        COMMENT "Merging static libraries into ${MERGED_LIB_PATH}"
        VERBATIM
    )
endif()

# Create custom target to trigger the merge
add_custom_target(finkit_stats_static ALL
    DEPENDS ${MERGED_LIB_PATH}
    COMMAND ${CMAKE_COMMAND} -E echo "Merged library created at: ${MERGED_LIB_PATH}"
)

# Create interface library for linking
add_library(finkit_stats_static_interface INTERFACE)
target_include_directories(finkit_stats_static_interface
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(finkit_stats_static_interface
    INTERFACE
        m
        ${MERGED_LIB_PATH}
)

# Create alias
add_library(finkit::stats ALIAS finkit_stats_static_interface)

# Install merged static library
install(FILES ${MERGED_LIB_PATH}
    DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include/stats
    FILES_MATCHING PATTERN "*.h"
)

# Install interface library target
install(TARGETS finkit_stats_static_interface
    EXPORT finkit-stats-targets
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
