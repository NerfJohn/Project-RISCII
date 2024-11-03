/*
 * main.cpp: Main elements/execution of the IR-backend program.
 */

// TODO- clean up this file once the dust settles...

#include <iostream>
#include "Device/GetOpt.h"

using namespace std;

//==============================================================================
// TODO- define once main is ready...
int main(int argc, char* argv[]) {
	// Process args.
	GetOpt cliArgs(argc, argv);

	// Display info on each.
	while(cliArgs.getOpt()) {
		// Display data.
		cout << "Value: " << cliArgs.m_value << endl;
		cout << "Arg: " << cliArgs.m_arg << endl;

		// Spacer between entries.
		cout << endl;
	}
}

