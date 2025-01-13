# CMake script for building "cprep" (RISCII compiler c-preprocessor).

# Std CMake preamble.
cmake_minimum_required(VERSION 3.14)
project(cprep)

################################################################################
# Update section with development
################################################################################

# Gather files/paths (globbing because i'm lazy/arrogant).
file(GLOB build_sources
	CprepProject/Device/*.cpp
	CprepProject/Os/*.cpp
	CprepProject/State/*.cpp
	CprepProject/Util/*.cpp
	CprepProject/main.cpp
)
file(GLOB build_includes CprepProject)

################################################################################
# ~Update section...
################################################################################

# Core sources/includes for build.
add_executable(cprep
	${shared_sources}
	${build_sources}
)
target_include_directories(cprep PUBLIC
	${build_includes}
)
