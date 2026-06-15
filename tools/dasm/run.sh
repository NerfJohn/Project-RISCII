#!/bin/bash

# (Internet one liner for getting script directory- no symlinks).
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Conduct build from known place- script's home directory.
cd ${SCRIPT_DIR}

# Run script/tool.
python src/main.py "$@"