# Example CMake script for cmake/bash driven project with Eclipse IDE.

# Std CMake preamble.
cmake_minimum_required(VERSION 3.14)
project(example)

################################################################################
# Update section with development
################################################################################

# Gather files/paths (globbing because i'm lazy/arrogant).
file(GLOB build_sources
	ExampleProject/mian.cpp
	ExampleProject/Foo/*.cpp
)
file(GLOB build_includes
	ExampleProject
)

################################################################################
# ~Update section...
################################################################################

# Core sources/includes for build.
add_executable(example
	${build_sources}
)
target_include_directories(example PUBLIC
	${build_includes}
)
