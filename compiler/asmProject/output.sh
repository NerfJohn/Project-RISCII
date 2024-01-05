# Nifty one-liner to get the script's directory (regardless of call location).
SRC_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Execute program with given args
$SRC_DIR/Debug/RisciiAssembler.exe "$@" 
