#!/bin/bash

# (Internet one liner for getting script directory (no symlinks).
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Conduct build from known place- cprep/script's home directory.
cd ${SCRIPT_DIR}

# Generate "version" for the program (not best, but will do for now).
(echo '#define APP_VERSION "'`git log -n 1 --format=%x76%h`'"') > ./ExampleProject/version.h

# Serial of commands to get cprep.exe in its parent directory.
cmake -S . -B build -G 'MSYS Makefiles' && cmake --build build && mv ./build/example.exe .
