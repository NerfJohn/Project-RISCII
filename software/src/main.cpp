/*
 * main.cpp
 */

#include <iostream>
#include <sstream>
#include <string>
#include "MsgLog.h"

using namespace std;

// TODO
void printInfo(void) {
	// Print directly to stdout to avoid MsgLog's rules/formatting.
	cout << "./quaidcc [options] <filename>" << endl;
	cout << endl;
	cout << "Compiles single C file for RISCII processor" << endl;
	cout << endl;
	cout << "Options:" << endl;
	cout << "-h      print this info and quit" << endl;
	cout << "-O1     enable \"win-win\" optimizations" << endl;
	cout << "-s      create assembly file" << endl;
	cout << "-v      enable verbose output" << endl;
	cout << "-Werror treat warnings as errors" << endl;

	// Generally a success... terminate well.
	exit(0);
}

// TODO
int main(int argc, char* argv[]) {
	// Long-term arguments.
	bool optFlag = false; // defaults to "optimizations off"
	bool asmFlag = false; // defaults to "no asm file made"
	string cFilename;

	// Parse arguments.
	if (argc <= 1) {MsgLog::logMsg(ERR, "Filename not provided"); /*exit*/}
	if (string(argv[1]) == "-h") {printInfo();}
	for (int i = 1; i < (argc - 1); i++) { // skip program and file names
		// Match/set flags (repeats allowed, but not unknown flags).
		string flag(argv[i]);
		if 		(flag == "-h") 		{printInfo();}
		else if (flag == "-O1") 	{optFlag = true;}
		else if (flag == "-s")		{asmFlag = true;}
		else if (flag == "-v")	   	{MsgLog::doVerbose(true);}
		else if (flag == "-Werror") {MsgLog::doWerror(true);}
		else {MsgLog::logMsg(ERR, "\"" + flag + "\" is not a valid flag");}
	}
	cFilename = argv[argc - 1];

	// INFO checkpoint.
	MsgLog::logMsg(INFO, "C Filename: " + cFilename);
	MsgLog::logMsg(INFO, string("optFlag = ") + ((optFlag) ? "true" : "false"));
	MsgLog::logMsg(INFO, string("asmFlag = ") + ((asmFlag) ? "true" : "false"));

	/* Compilation stages */

	return 0;
}
