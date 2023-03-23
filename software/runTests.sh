# Run Tests Script- runs tests on the software design
# by John Eslinger

# Folder locations to determine procedure for testing.
FEAT_DIR="./tests/feature_tests/"
FAIL_DIR="./tests/failure_tests/"
DETR_DIR="./tests/detour_tests/"

# Name of the software executable.
EXE_NAME="quaidcc.exe"

# Path to hardware simulator bash script.
TO_SIM_DIR="./../hardware"

# -- Flag Handling -- #

# Respond to help flag.
if [ "$#" -gt 0 ] && [ "$1" == "-h" ]; then
	echo "./runTests.sh [-h] <directory>"
	exit 0
fi

# Ensure a directory is provided to use.
if [ "$#" -lt 1 ] || [ ! -d "$1" ]; then
	echo "First argument not an existing directory! Exiting..."
	exit 1
fi

# -- Executable Preparation -- #

# Ensure software is built for testing.
./runBuild.sh
if [ $? -ne 0 ]; then
	echo "Build of tested software failed! Exiting..."
	exit 1
fi

# Ensure expected executable exists.
if [ ! -f ./$EXE_NAME ]; then
	echo "Couldn\'t find ./$EXE_NAME! Exiting..."
	exit 1
fi

# -- Test Execution -- #

# Get list of tests to run.
TESTS=$(find $1 -name "*.c")

# Setup for test iteration.
echo "Running tests from \"$1\"..."
TESTS_RUN=0
TESTS_PASSED=0

# Keep track of time taken to run tests.
TIME_START=$(date +%s)

# Iterate through tests.
for TEST in $TESTS; do
	# Clean previous files.
	TEST_HEX=${TEST%.c}.hex
	rm -f $TEST_HEX
	
	# Count test as run.
	TESTS_RUN=$((TESTS_RUN+1))
	
	# Compilation procedure determined by location (ie infers test "type").
	if [[ $TEST == *$FAIL_DIR* ]]; then
		# Run test file with strict checking- check it failed as expected.
		./$EXE_NAME -Werror $TEST > /dev/null
		if [ $? -eq 0 ] || [ -f $TEST_HEX ]; then
			echo "FAILED! ($TEST) Test compiled or software returned 0..."
			continue
		fi
	elif [[ $TEST == *$DETR_DIR* ]]; then
		# Run test file with optimizations- check it was successful.
		./$EXE_NAME -O1 $TEST > /dev/null
		if [ $? -ne 0 ] || [ ! -f $TEST_HEX ]; then
			echo "FAILED! ($TEST) Test didn't compile or returned with error..."
			continue
		fi
	else 								# Default to "feature test" procedure.
		# Run test file- check it was successful.
		./$EXE_NAME $TEST > /dev/null
		if [ $? -ne 0 ] || [ ! -f $TEST_HEX ]; then
			echo "FAILED! ($TEST) Test didn't compile or returned with error..."
			continue
		fi
	fi
	
	# Check created .hex files work as expected (not reached by failure tests).
	if [ -f $TEST_HEX ]; then
		# Run result as hardware test.
		(cat $TEST_HEX > $TO_SIM_DIR/temp.hex; cd $TO_SIM_DIR; ./runSim.sh temp.hex -test) > /dev/null
		if [ $? -ne 0 ]; then
			echo FAILED! \($TEST\) runSim.sh failed...
			rm -f $TO_SIM_DIR/temp.hex
			continue
		fi
		rm -f $TO_SIM_DIR/temp.hex
	
		# Check result exists (and wrote 1 at address 0x0).
		if [ ! -f $TO_SIM_DIR/temp.out ]; then
			echo FAILED! \($TEST\) .out file not created...
			continue
		fi
		if [[ $(head -n 1 $TO_SIM_DIR/temp.out) -ne 1 ]]; then
			echo FAILED! \($TEST\) Result was $(head -n 1 $TO_SIM_DIR/temp.out)... \(in $(head -n 2 $TO_SIM_DIR/temp.out | tail -n 1) cycles\)
			rm -f $TO_SIM_DIR/temp.out
			continue
		fi
		rm -f $TO_SIM_DIR/temp.out
	fi
	
	# Test passed if this statement is reached!
	echo passed! \($TEST\)
	TESTS_PASSED=$((TESTS_PASSED+1))
done

# Report result/end of tests.
DURATION=$(($(date +%s) - $TIME_START))
echo Finished tests in $DURATION seconds. $TESTS_PASSED / $TESTS_RUN passed.