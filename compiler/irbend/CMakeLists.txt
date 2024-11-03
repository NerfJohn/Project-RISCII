# CMake script for building/testing "irbend" (RISCII compiler backend).

# Std CMake preamble.
cmake_minimum_required(VERSION 3.14)
project(irbend)

# GoogleTest requires at least C++14.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure run action is defined.
if(NOT DEFINED RUN)
message("'RUN' not defined- default is 'Build'...")
set(RUN "Build")
endif()

# Validate/declare run action- must be "Build" or "Test".
if(NOT (${RUN} STREQUAL "Build" OR ${RUN} STREQUAL "Test"))
message("Unknown run action '${RUN}', aborting...")
return()
endif()
message("Run action '${RUN}'")

################################################################################
# Update section with development
################################################################################

# Gather files/paths (globbing because i'm lazy/arrogant).
file(GLOB shared_sources 
	IrbendProject/Device/*.cpp
)
file(GLOB build_sources
	IrbendProject/main.cpp
)
file(GLOB test_sources
	tests/Device/*.cpp
)
file(GLOB build_includes IrbendProject)
file(GLOB test_includes .)

################################################################################
# ~Update section...
################################################################################

# Run the appropriate build procedure.
if(${RUN} STREQUAL "Build")
# "Build"- setup system for building "irbend.exe".

# Core sources/includes for build.
add_executable(irbend
	${shared_sources}
	${build_sources}
)
target_include_directories(irbend PUBLIC
	${build_includes}
)

else()
# "Test"- setup system for testing unit tests of irbend source code.

# Core sources/includes for test.
add_executable(irbend
	${shared_sources}
	${test_sources}
)
target_include_directories(irbend PUBLIC
	${build_includes}
	${test_includes}
)

# Get GoogleTest resources to run unit tests.
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Get ready for testing (idk- following GoogleTest example). 
enable_testing()

# Link GoogleTest with unit tests and source code.
target_link_libraries(
  irbend
  GTest::gtest_main
)

# Find/link to tests (again- idk).
include(GoogleTest)
gtest_discover_tests(irbend)

endif()
