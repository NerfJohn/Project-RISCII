# (Internet one liner for getting script directory (no symlinks).
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Conduct build from known place- cprep/script's home directory.
cd ${SCRIPT_DIR}

# Clean builds ONLY.
rm -rf build cprep.exe

# Generate "version" for the program (not best, but will do for now).
(echo '#define APP_VERSION "'`git log -n 1 --format=%x76%h`'"') > ./CprepProject/version.h

# Serial of commands to get cprep.exe in its parent directory.
cmake -S . -B build && cmake --build build && mv ./build/Debug/cprep.exe .
