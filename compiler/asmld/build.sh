# (Internet one liner for getting script directory (no symlinks).
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Conduct build from known place- asmld/script's home directory.
cd ${SCRIPT_DIR}

# Clean builds ONLY.
rm -rf build asmld.exe

# Serial of commands to get asmld.exe in its parent directory.
cmake -S . -B build -DRUN=Build && cmake --build build && mv ./build/Debug/asmld.exe .
