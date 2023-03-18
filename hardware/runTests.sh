# Run Tests Script- runs tests on the hardware design
# by John Eslinger

# Respond to help flag.
if [ "$#" -gt 0 ] && [ "$1" == "-h" ]; then
	echo "./runTests.sh [-h] <directory>"
	exit 0
fi

# Ensure a directory is provided to run.
if [ "$#" -lt 1 ] || [ ! -d "$1" ]; then
	echo First argument not an existing directory! Exiting...
	exit 1
fi

# Get list of tests to run.
TESTS=$(find $1 -name "*.asm")

# Setup for test iteration.
echo Running tests from "$PWD"...
TESTS_RUN=0
TESTS_PASSED=0

# Keep track of time taken to run tests.
TIME_START=$(date +%s)

# Iterate through the tests.
for TEST in $TESTS;
do
	# Clean previous files.
	TEST_HEX=${TEST%.asm}.hex
	TEST_OUT=${TEST%.asm}.out
	rm -f $TEST_HEX
	rm -f $TEST_OUT
	
	# Count test as run.
	TESTS_RUN=$((TESTS_RUN+1))

	# Try to assemble the file.
	python ./scripts/quickAsm.py $TEST
	if [ $? -ne 0 ]; then
		echo FAILED! \($TEST\) quickAsm.py failed...
		continue
	fi
	
	# Try to run the file.
	./runSim.sh $TEST_HEX -test > /dev/null
	if [ $? -ne 0 ]; then
		echo FAILED! \($TEST\) runSim.sh failed...
		continue
	fi
	
	# Check result exists (and wrote 1 at address 0x0).
	if [ ! -f $TEST_OUT ]; then
		echo FAILED! \($TEST\) .out file not created...
		continue
	fi
	if [[ $(head -n 1 $TEST_OUT) -ne 1 ]]; then
		echo FAILED! \($TEST\) Result was $(head -n 1 $TEST_OUT)... \(in $(head -n 2 $TEST_OUT | tail -n 1) cycles\)
		continue
	fi
	
	# Test passed!
	echo passed! \($TEST\) in $(head -n 2 $TEST_OUT | tail -n 1) cycles
	TESTS_PASSED=$((TESTS_PASSED+1))
	
done

# Report result/end of tests.
DURATION=$(($(date +%s) - $TIME_START))
echo Finished tests in $DURATION seconds. $TESTS_PASSED / $TESTS_RUN passed.
