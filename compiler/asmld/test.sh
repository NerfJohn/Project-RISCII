# (Internet one liner for getting script directory (no symlinks).
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Conduct test from known place- asmld/script's home directory.
cd ${SCRIPT_DIR}

# Clean tests ONLY.
rm -rf build

# Serial of commands to run tests on asmld source code.
cmake -S . -B build -DRUN=Test && cmake --build build && cd build && ctest
