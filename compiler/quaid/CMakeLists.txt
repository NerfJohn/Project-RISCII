# Plug-and-play parameters.
set(PROJ_NAME "QuaidProject")
set(EXEC_NAME "quaid")

# Std CMake preamble.
cmake_minimum_required(VERSION 3.14)
project(${EXEC_NAME})

# Gather files/paths (globbing because i'm lazy/arrogant).
file(GLOB build_sources
	${PROJ_NAME}/main.cpp
	../_shared/Common/Device/*.cpp
	../_shared/Common/Os/*.cpp
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
