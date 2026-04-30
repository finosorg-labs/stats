#
# tests/CMakeLists.txt - Test configuration for stats module
#

# Test executable
add_executable(stats_tests
    test_framework.c
    test_main.c
    test_stats.c
    test_mean_var.c
    test_kahan_sum.c
    test_welford.c
)

# Link with stats library
target_link_libraries(stats_tests
    PRIVATE
        finkit_stats_static
        m
)

target_compile_definitions(stats_tests PRIVATE
    FC_STATIC_BUILD
)

# Include directories
target_include_directories(stats_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/modules/platform/include
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Compiler flags for tests
target_compile_options(stats_tests PRIVATE -Wall -Wextra -Wpedantic)

# Add AVX2 flags if supported
if(FC_SIMD_LEVEL STREQUAL "AVX2" OR FC_SIMD_LEVEL STREQUAL "AVX512")
    target_compile_options(stats_tests PRIVATE -mavx2 -mfma)
endif()

# Run tests with ctest
add_test(
    NAME "stats_tests"
    COMMAND stats_tests
)

# Installation
install(TARGETS stats_tests
    RUNTIME DESTINATION bin
)
