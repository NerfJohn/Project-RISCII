# Run Build Script- builds software design
# by John Eslinger

# Build arguments + name of created executable
EXE_NAME="quaidcc.exe"
BUILD_ARGS="-O0 -Wall"

# Name of scan table mock-up.
SCAN_MOCK="./scripts/scanTableMock.txt"

# -- Flag Handling -- #

# Respond to help flag.
if [ "$#" -gt 0 ] && [ "$1" == "-h" ]; then
	echo "./runBuild.sh [-h] [-rebuild]"
	exit 0
fi

# Check if caller wants to force a clean build (vs. checking if one's needed).
IS_TEST=0
if [ "$#" -gt 1 ] || [ "$1" == "-rebuild" ]; then
	echo "Rebuild requested- rebuilding software..."
	DO_REBUILD=1
fi

# -- C++ Software Compilation -- #

# Create .exe for timestamping build/storing build data as needed.
if [ ! -f $EXE_NAME ]; then
	echo "No previous build detected- building software..."
	DO_REBUILD=1
fi

# Rebuild ScanTable.h using script if updated (or rebuild called for).
if [ $DO_REBUILD ] || [ $SCAN_MOCK -nt $EXE_NAME ]; then
	echo Rebuilding ScanTable.h from $SCAN_MOCK...
	rm -rf ./src/ScanTable.h # Clear previous build of ScanTable.h
	
	# Generate ScanTable.h in current directory.
	python ./scripts/genScanTable.py $SCAN_MOCK
	if [ $? -ne 0 ]; then
		echo Failed to create ScanTable.h! Exiting...
		rm -f $EXE_NAME
		exit 1
	fi
	
	# Move ScanTable.h to src directory.
	cp ScanTable.h ./src/
	rm -rf ScanTable.h
fi

# Only perform these checks if unsure whether to rebuild software.
if [ ! $DO_REBUILD ]; then	
	# Get source files to check for updated source files.
	SRC_FILES=$(find . -name "*.cpp" -o -name "*.h")
	
	# Re-build if any files have been created/modified. (DOESN'T DETECT DELETES)
	for ENTRY in $SRC_FILES; do
		# If newer file found, rebuild.
		if [ $ENTRY -nt $EXE_NAME ]; then
			echo "Changes detected- rebuilding software..."
			DO_REBUILD=1
			break
		fi
	done
fi

# Rebuild software if needed/asked for.
if [ $DO_REBUILD ]; then
	echo Rebuilding main program...

	# Get source files for building.
	SRC_FILES=$(find . -name "*.cpp")
	
	# Get included directories.
	for DIR in $(find . -type d); do
		INC_DIRS="$INC_DIRS -I $DIR"
	done
	
	# Build the software.
	g++ $BUILD_ARGS -o $EXE_NAME $INC_DIRS $SRC_FILES
	
	# Report/cleanup if errors occur.
	if [ $? -ne 0 ]; then
		echo Failed to build! Exiting...
		rm -f $EXE_NAME
		exit 1
	fi
fi
