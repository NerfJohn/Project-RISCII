# Run Simulation Script- compiles/runs simulated hardware
# by John Eslinger

# Respond to help flag.
if [ "$#" -gt 0 ] && [ "$1" == "-h" ]; then
	echo "./runSim.sh [-h] <input .hex file> [-test]"
	exit 0
fi

# Ensure a .hex file is provided to run.
if [ "$#" -lt 1 ] || [ ! -f "$1" ] || [[ $1 != *.hex ]]; then
	echo First argument not an existing .hex file! Exiting...
	exit 1
fi

# Check for test flag.
IS_TEST=0
if [ "$#" -gt 1 ] || [ "$2" == "-test" ]; then
	IS_TEST=1
fi

# Get source files to check for update.
SRC_FILES=$(find . -name "*.sv" -o -name "*.v")

# Re-compile if any files have been created/updated (DOESN'T DETECT DELETES)
for ENTRY in $SRC_FILES;
do
	if [ $ENTRY -nt "./work/_info" ]; then
		# Updated file found- recompile.
		echo Changes detected. recompiling hardware...
		vlog $SRC_FILES
		
		# Exit if recompiling failed.
		if [ $? -ne 0 ]; then
			echo Failed to recompile! Exiting...
			exit 1
		fi
		
		# Recompiled- continue with simulation.
		break
	fi
done

# Setup input/output files.
cat $1 > sim_input
rm -f sim_output

# Run the simulation.
vsim -c -do "run -all; quit" -g IS_TEST=$IS_TEST sim_top
if [ $? -ne 0 ]; then
	echo Error during simulation! Exiting...
	exit 1
fi

# Clean up created/used files.
rm -f sim_input
rm -f transcript
if [ -f "sim_output" ]; then
	cat sim_output > ${1%.hex}.out
	rm -f sim_output
else
	# File should've been created- exit with error.
	exit 1
fi

# Exit successfully
exit 0
