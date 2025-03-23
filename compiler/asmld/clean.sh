#!/bin/bash

# Plug-and-play parameters.
PROJ_NAME="AsmldProject"
EXEC_NAME="asmld"

# (Internet one liner for getting script directory- no symlinks).
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Conduct build from known place- script's home directory.
cd ${SCRIPT_DIR}

# Clean builds ONLY.
rm -rf build ${EXEC_NAME}.exe
