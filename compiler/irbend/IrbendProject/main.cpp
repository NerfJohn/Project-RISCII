/*
 * main.cpp: Main elements/execution of the IR-backend program.
 */

// TODO- clean up this file once the dust settles...

#include "Device/GetOpt.h"
#include "Device/Print.h"
#include "Device/Terminate.h"

#include "Device/File.h"
#include "Domain/LexState_e.h"

using namespace std;

//==============================================================================
// TODO- define once main is ready...
int main(int argc, char* argv[]) {
	// Process args.
	GetOpt cliArgs(argc, argv);

	// Display info on each.
	while(cliArgs.getOpt()) {
		// Display data.
		string val = string("Value: ") + cliArgs.m_value;
		string arg = string("Arg: ") + cliArgs.m_arg;
		Print::inst().cli(val);
		Print::inst().cli(arg);

		// Spacer between entries.
		Print::inst().cli("");
	}

	File::inst().open("test.sh", FILE_OP_READ);
	while(File::inst().peek() != 0xFF) {
		Print::inst().cli(to_string((File::inst().pop())));
	}

	// Successful run.
	Terminate_silent(RET_SUCCESS);
}

