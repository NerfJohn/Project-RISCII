#!/bin/bash

# (Internet one liner for getting script directory- no symlinks).
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Conduct build from known place- script's home directory.
cd ${SCRIPT_DIR}

# Array of script names
scripts=("../cprep/build.sh" "../asmld/build.sh" "../quaid/build.sh")

# Loop through the scripts and execute each one
for script in "${scripts[@]}"
do
  bash "$script"
  
  # Check the exit status of each script
  if [ $? -ne 0 ]; then
    echo "Failed to build- exiting..."
    exit 1 # Terminate immediately if a script fails
  fi
done


# Copy compiler parts to common folder.
echo "All exe built- generating folder..."
rm -rf ./bin
mkdir bin
cp ../cprep/cprep.exe ./bin
cp ../asmld/asmld.exe ./bin
cp ../quaid/quaid.exe ./bin