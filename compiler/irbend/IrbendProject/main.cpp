/*
 * main.cpp: Main elements/execution of the IR-backend program.
 */

// TODO- clean up this file once the dust settles...

#include <iostream>
#include "Device/GetOpt.h"

#include "Device/Print.h"
#include "Os/OsExit.h"
#include "Os/OsStdout.h"

using namespace std;

//==============================================================================
// TODO- define once main is ready...
int main(int argc, char* argv[]) {
	// Process args.
	GetOpt cliArgs(argc, argv);

	Print::inst().log(LOG_ERROR, "error 1");
	Print::inst().setLogLevel(LOG_ERROR);
	Print::inst().log(LOG_ERROR, "error 2");

	Print::inst().log(LOG_SILENT, "silence");

	// Display info on each.
	while(cliArgs.getOpt()) {
		// Display data.
		cout << "Value: " << cliArgs.m_value << endl;
		cout << "Arg: " << cliArgs.m_arg << endl;

		// Spacer between entries.
		cout << endl;
	}

	OsExit_exit(RET_SUCCESS);
}

