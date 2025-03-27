/*
 * Cmd.cpp: Standardized method of calling command line.
 */

#include "Common/Device/Terminate.h"
#include "Os/OsCmd.h"

#include "Device/Cmd.h"

//==============================================================================
// Calls command on command line. Is blocking.
int Cmd_run(std::string cmd) {
	// Code to return.
	int retCode = 0;

	// Attempt to run command.
	if (OsCmd_run(cmd, retCode) == RET_ERR_ERROR) {
		Terminate::inst().assert("run() failed to run process");
	}

	// Otherwise, return result of run program.
	return retCode;
}
