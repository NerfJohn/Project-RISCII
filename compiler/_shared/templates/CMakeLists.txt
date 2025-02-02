# Plug-and-play parameters.
set(PROJ_NAME "TODO- Project name/folder here")
set(EXEC_NAME "TODO- Executable name (without .exe) here")

# Std CMake preamble.
cmake_minimum_required(VERSION 3.14)
project(${EXEC_NAME})

# Gather files/paths (globbing because i'm lazy/arrogant).
file(GLOB build_sources
	${PROJ_NAME}/main.cpp

	# TODO- ../_shared/Common/<directory or cpps here>
)
file(GLOB build_includes
	${PROJ_NAME}

	# TODO- ../_shared
)

# Add souces/includes to build.
add_executable(${EXEC_NAME}
	${build_sources}
)
target_include_directories(${EXEC_NAME} PUBLIC
	${build_includes}
)
