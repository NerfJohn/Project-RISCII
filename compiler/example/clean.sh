#!/bin/bash

# (Internet one liner for getting script directory (no symlinks).
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Conduct build from known place- cprep/script's home directory.
cd ${SCRIPT_DIR}

# Clean builds ONLY.
rm -rf build example.exe