# Plug-and-play parameters.
set(PROJ_NAME "CcompProject")
set(EXEC_NAME "ccomp")

# Std CMake preamble.
cmake_minimum_required(VERSION 3.14)
project(${EXEC_NAME})

# Gather files/paths (globbing because i'm lazy/arrogant).
file(GLOB build_sources
	${PROJ_NAME}/main.cpp
	${PROJ_NAME}/Ds/*.cpp
	${PROJ_NAME}/State/*.cpp
	../_shared/Common/Device/*.cpp
	../_shared/Common/Os/*.cpp
	../_shared/Common/Util/*.cpp
)
file(GLOB build_includes
	${PROJ_NAME}
	../_shared
)

# Add souces/includes to build.
add_executable(${EXEC_NAME}
	${build_sources}
)
target_include_directories(${EXEC_NAME} PUBLIC
	${build_includes}
)
