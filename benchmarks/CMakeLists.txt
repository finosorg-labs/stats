#
# benchmarks/CMakeLists.txt - Benchmark configuration for stats module
#

# Benchmark framework library
if(NOT TARGET bench_framework)
    add_library(bench_framework OBJECT
        bench_framework.c
    )

    target_include_directories(bench_framework
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/modules/platform/include
            ${CMAKE_SOURCE_DIR}/modules/matrix/include
    )

    target_link_libraries(bench_framework
        PUBLIC
            finkit_stats_static
    )
endif()

# Benchmark executable
add_executable(stats_benchmarks
    bench_main.c
    bench_stats.c
    bench_mean_var.c
    bench_kahan_sum.c
    bench_welford.c
    bench_covariance.c
    bench_correlation.c
    bench_quantile.c
    bench_rolling.c
    bench_rank.c
    bench_ema.c
    bench_moments.c
    bench_zscore.c
)

# Link with benchmark framework and stats library
target_link_libraries(stats_benchmarks
    PRIVATE
        bench_framework
        finkit_stats_static
        m
)

target_compile_definitions(stats_benchmarks PRIVATE
    FC_STATIC_BUILD
)

# Include directories
target_include_directories(stats_benchmarks
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/modules/platform/include
        ${CMAKE_SOURCE_DIR}/modules/matrix/include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compiler flags for benchmarks
target_compile_options(stats_benchmarks PRIVATE -Wall -Wextra -Wpedantic -ffp-contract=off)

# Use the same SIMD flags as the library build
# This ensures benchmarks can run on the target CPU
if(FC_SIMD_CFLAGS)
    set_source_files_properties(bench_mean_var.c PROPERTIES COMPILE_FLAGS "${FC_SIMD_CFLAGS}")
    set_source_files_properties(bench_kahan_sum.c PROPERTIES COMPILE_FLAGS "${FC_SIMD_CFLAGS}")
    set_source_files_properties(bench_welford.c PROPERTIES COMPILE_FLAGS "${FC_SIMD_CFLAGS}")
    set_source_files_properties(bench_covariance.c PROPERTIES COMPILE_FLAGS "${FC_SIMD_CFLAGS}")
    set_source_files_properties(bench_correlation.c PROPERTIES COMPILE_FLAGS "${FC_SIMD_CFLAGS}")
    set_source_files_properties(bench_quantile.c PROPERTIES COMPILE_FLAGS "${FC_SIMD_CFLAGS}")
    set_source_files_properties(bench_rolling.c PROPERTIES COMPILE_FLAGS "${FC_SIMD_CFLAGS}")
    set_source_files_properties(bench_rank.c PROPERTIES COMPILE_FLAGS "${FC_SIMD_CFLAGS}")
    set_source_files_properties(bench_ema.c PROPERTIES COMPILE_FLAGS "${FC_SIMD_CFLAGS}")
    set_source_files_properties(bench_moments.c PROPERTIES COMPILE_FLAGS "${FC_SIMD_CFLAGS}")
    set_source_files_properties(bench_zscore.c PROPERTIES COMPILE_FLAGS "${FC_SIMD_CFLAGS}")
endif()

# Installation
install(TARGETS stats_benchmarks
    RUNTIME DESTINATION bin
)
