#!/bin/bash

# Plug-and-play parameters.
PROJ_NAME="TODO- Project name/folder here"
EXEC_NAME="TODO- Executable name (without .exe) here"

# (Internet one liner for getting script directory- no symlinks).
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Conduct build from known place- script's home directory.
cd ${SCRIPT_DIR}

# Generate "version" for the program (not best, but will do for now).
(echo '#define APP_VERSION "'`git log -1 --format=%cd --date=format:v%Y%m%d`'"') > ./${PROJ_NAME}/version.h

# Serial of commands to get executable in home directory.
cmake -S . -B build -G 'MSYS Makefiles' && cmake --build build && mv ./build/${EXEC_NAME}.exe .
