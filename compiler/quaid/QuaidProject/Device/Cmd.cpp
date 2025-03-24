/*
 * Cmd.cpp: TODO
 */

#include "Common/Device/Terminate.h"
#include "Os/OsCmd.h"

#include "Device/Cmd.h"

// TODO
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
