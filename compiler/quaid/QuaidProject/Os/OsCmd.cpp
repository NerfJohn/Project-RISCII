/*
 * OsCmd.cpp: TODO
 */

#include <windows.h>

#include "Os/OsCmd.h"

// TODO
RetErr_e OsCmd_run(std::string& cmd, int& res) {
	// Prep command to be run.
	LPSTR cliCmd = &(cmd[0]);

	// Set size of the structures (idk- copy/paste coding).
	STARTUPINFO         si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb          = sizeof(si);
	si.hStdOutput  = GetStdHandle(STD_OUTPUT_HANDLE);
	si.hStdError   = GetStdHandle(STD_ERROR_HANDLE);
	si.dwFlags    |= STARTF_USESTDHANDLES;

	// Run command to completion.
	unsigned long int code;
	BOOL procRes = CreateProcessA( 0, cliCmd, 0, 0, TRUE, 0, 0, 0, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);
	GetExitCodeProcess(pi.hProcess, &code);
	res = (int)(code);

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // Translate result.
    return (procRes == TRUE) ? RET_ERR_NONE : RET_ERR_ERROR;
}
