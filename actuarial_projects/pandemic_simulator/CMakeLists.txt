# Pandemic Risk Simulator - CMakeLists.txt
# High-performance pandemic simulation system
cmake_minimum_required(VERSION 3.16)
project(PandemicSimulator VERSION 1.0.0 LANGUAGES C CXX)

# Set C/C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_RELEASE "-O3 -march=native -flto -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -DNDEBUG")

# Find required packages
find_package(OpenMP REQUIRED)
find_package(MPI QUIET)

# Find GSL (GNU Scientific Library)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSL REQUIRED gsl)

# CUDA support (optional)
option(USE_CUDA "Enable CUDA GPU acceleration" ON)
if(USE_CUDA)
    find_package(CUDA QUIET)
    if(CUDA_FOUND)
        enable_language(CUDA)
        set(CMAKE_CUDA_STANDARD 11)
        set(CMAKE_CUDA_STANDARD_REQUIRED ON)
        add_definitions(-DUSE_CUDA)
    else()
        set(USE_CUDA OFF)
        message(WARNING "CUDA not found, GPU acceleration disabled")
    endif()
endif()

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${GSL_INCLUDE_DIRS}
)

# Link directories
link_directories(${GSL_LIBRARY_DIRS})

# Source files
set(C_SOURCES
    src/main.c
    src/models/epidemiological/seir_model.c
    src/models/economic/input_output_model.c
    src/models/actuarial/pandemic_loss_model.c
    src/models/distributed/pandemic_cluster.c
    src/visualization/plotting.c
    src/utils/data_loader.c
    src/utils/statistics.c
)

set(CXX_SOURCES
    src/models/epidemiological/agent_based_model.cpp
    src/models/economic/network_model.cpp
)

# CUDA sources (if CUDA is available)
if(USE_CUDA)
    set(CUDA_SOURCES
        src/gpu/pandemic_gpu.cu
        src/gpu/cuda_kernels.cu
        src/gpu/agent_simulation.cu
    )
endif()

# MPI sources (if MPI is available)
if(MPI_FOUND)
    set(MPI_SOURCES
        src/distributed/mpi_communication.c
        src/distributed/parallel_simulation.c
    )
    add_definitions(-DUSE_MPI)
endif()

# Create main executable
add_executable(pandemic_simulator
    ${C_SOURCES}
    ${CXX_SOURCES}
    ${CUDA_SOURCES}
    ${MPI_SOURCES}
)

# Set CUDA separable compilation
if(USE_CUDA)
    set_target_properties(pandemic_simulator PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
        CUDA_RESOLVE_DEVICE_SYMBOLS ON
    )
endif()

# Link libraries
target_link_libraries(pandemic_simulator
    OpenMP::OpenMP_C
    ${GSL_LIBRARIES}
    m  # Math library
    dl  # Dynamic loading
)

# CUDA libraries
if(USE_CUDA)
    target_link_libraries(pandemic_simulator
        cudart
        cublas
        curand
        cusolver
    )
endif()

# MPI libraries
if(MPI_FOUND)
    target_link_libraries(pandemic_simulator MPI::MPI_C)
endif()

# Create static library
add_library(pandemic_simulator_lib STATIC
    ${C_SOURCES}
    ${CXX_SOURCES}
    ${CUDA_SOURCES}
    ${MPI_SOURCES}
)

target_link_libraries(pandemic_simulator_lib
    OpenMP::OpenMP_C
    ${GSL_LIBRARIES}
    m
    dl
)

if(USE_CUDA)
    target_link_libraries(pandemic_simulator_lib
        cudart
        cublas
        curand
        cusolver
    )
endif()

if(MPI_FOUND)
    target_link_libraries(pandemic_simulator_lib MPI::MPI_C)
endif()

# Set library properties
set_target_properties(pandemic_simulator_lib PROPERTIES
    OUTPUT_NAME pandemic_simulator
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Installation
install(TARGETS pandemic_simulator
    RUNTIME DESTINATION bin
)

install(TARGETS pandemic_simulator_lib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include/pandemic_simulator
    FILES_MATCHING PATTERN "*.h"
)

# Testing
option(BUILD_TESTS "Build test executables" ON)
if(BUILD_TESTS)
    enable_testing()

    # Test executable
    add_executable(test_pandemic_simulator
        tests/test_main.c
        tests/models/test_epidemiological_model.c
        tests/models/test_economic_model.c
        tests/models/test_actuarial_model.c
    )

    target_link_libraries(test_pandemic_simulator
        pandemic_simulator_lib
    )

    add_test(NAME pandemic_simulator_tests
        COMMAND test_pandemic_simulator
    )
endif()

# Benchmarking
option(BUILD_BENCHMARKS "Build benchmark executables" ON)
if(BUILD_BENCHMARKS)
    add_executable(benchmark_pandemic_simulator
        benchmarks/benchmark_main.c
        benchmarks/benchmark_epidemiological.c
        benchmarks/benchmark_economic.c
        benchmarks/benchmark_actuarial.c
    )

    target_link_libraries(benchmark_pandemic_simulator
        pandemic_simulator_lib
    )
endif()

# Documentation
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    option(BUILD_DOCS "Build documentation" ON)
    if(BUILD_DOCS)
        set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/docs/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    endif()
endif()

# Packaging
set(CPACK_PACKAGE_NAME "pandemic-simulator")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "High-performance pandemic risk assessment system")
set(CPACK_PACKAGE_VENDOR "Pandemic Risk Simulator Team")
set(CPACK_PACKAGE_CONTACT "info@pandemic-simulator.org")

# Debian packaging
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libgsl23, libopenmpi3")
set(CPACK_DEBIAN_PACKAGE_SECTION "science")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

include(CPack)

# Print configuration summary
message(STATUS "Pandemic Simulator Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  C++ Compiler: ${CMAKE_CXX_COMPILER}")
if(USE_CUDA)
    message(STATUS "  CUDA Compiler: ${CMAKE_CUDA_COMPILER}")
    message(STATUS "  CUDA Version: ${CUDA_VERSION}")
else()
    message(STATUS "  CUDA: Disabled")
endif()
if(MPI_FOUND)
    message(STATUS "  MPI: Enabled (${MPI_C_VERSION})")
else()
    message(STATUS "  MPI: Disabled")
endif()
message(STATUS "  OpenMP: Enabled")
message(STATUS "  GSL: ${GSL_VERSION}")
message(STATUS "  Tests: ${BUILD_TESTS}")
message(STATUS "  Benchmarks: ${BUILD_BENCHMARKS}")
if(DOXYGEN_FOUND)
    message(STATUS "  Documentation: Enabled")
else()
    message(STATUS "  Documentation: Disabled (Doxygen not found)")
endif()

# Custom targets
add_custom_target(run-tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_pandemic_simulator
    COMMENT "Running all tests"
)

add_custom_target(run-benchmarks
    COMMAND benchmark_pandemic_simulator
    DEPENDS benchmark_pandemic_simulator
    COMMENT "Running performance benchmarks"
)

add_custom_target(format
    COMMAND find src include tests benchmarks -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
    COMMENT "Formatting source code"
)

add_custom_target(lint
    COMMAND find src include tests benchmarks -name "*.c" -o -name "*.cpp" | xargs cppcheck --enable=all --std=c11 --std=c++17 --quiet
    COMMENT "Running static analysis"
)

add_custom_target(check
    COMMAND ${CMAKE_MAKE_PROGRAM} format
    COMMAND ${CMAKE_MAKE_PROGRAM} lint
    COMMAND ${CMAKE_MAKE_PROGRAM} run-tests
    COMMENT "Running full quality check"
)

# Uninstall target
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)