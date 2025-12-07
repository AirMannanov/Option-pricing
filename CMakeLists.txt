cmake_minimum_required(VERSION 3.15)
project(option_pricing VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Library target: libpricing
add_library(pricing STATIC
    src/models/BlackScholesModel.cpp
)

target_include_directories(pricing PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# CLI application target
add_executable(option_pricer_cli
    src/cli/main.cpp
)

target_link_libraries(option_pricer_cli
    pricing
)

# Testing
enable_testing()

# Catch2 will be added later
# For now, we just set up the structure

