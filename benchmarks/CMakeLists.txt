#
# benchmarks/CMakeLists.txt - Benchmark configuration for stats module
#

# Benchmark framework library
add_library(bench_framework OBJECT
    bench_framework.c
)

target_include_directories(bench_framework
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/modules/platform/include
)

target_link_libraries(bench_framework
    PUBLIC
        finkit_stats_static
)

# Benchmark executable
add_executable(stats_benchmarks
    bench_main.c
    bench_stats.c
    bench_mean_var.c
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
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compiler flags for benchmarks
target_compile_options(stats_benchmarks PRIVATE -Wall -Wextra -Wpedantic -ffp-contract=off)

# Installation
install(TARGETS stats_benchmarks
    RUNTIME DESTINATION bin
)
