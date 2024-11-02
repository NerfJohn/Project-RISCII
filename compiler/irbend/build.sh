# (Internet one liner for getting script directory (no symlinks).
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Conduct build from known place- irbend/script's home directory.
cd ${SCRIPT_DIR}

# Clean builds ONLY.
rm -rf build irbend.exe

# Serial of commands to get irbend.exe in its parent directory.
cmake -S . -B build -DRUN=Build && cmake --build build && mv ./build/Debug/irbend.exe .
