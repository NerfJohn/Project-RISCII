/*
 * main.cpp
 */

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "ScanTable.h"

#include "ScanToken.h"

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
vector<ScanToken> scanFile(string cFilename) {
	MsgLog::logMsg(MSG_INFO, "Scanning...");

	// Prepare file to be read character by character.
	FILE* fptr = fopen(cFilename.c_str(), "r");
	if (!fptr) {MsgLog::logMsg(MSG_ERR, "Failed to open \"" + cFilename + "\"");}

	// Preset important scanning variables.
	uint8_t state = SCAN_START;	// Keep track of when/what to make a token
	string buffer;				// Record scanned chars for tokens
	uint32_t lineNum = 1;		// Record lines for warnings/errors
	vector<ScanToken> retTkns;	// List of tokens scanned in

	// Process every character (including the EOF character).
	char inChar;
	do {
		// Pop the next char into the buffer.
		inChar = fgetc(fptr);
		buffer += inChar;

		// (Minor Windows support: discard carriage returns.)
		if (inChar == '\r') {
			buffer.pop_back();	// Remove from buffer
			continue; 			// Discard by moving on
		}

		// Ensure char is part of scan table's accepted language.
		if(!ScanTable::isValidChar(inChar)) {
			string val = to_string((int)(inChar));	// Report as visible int
			MsgLog::logMsg(MSG_ERR, lineNum, "Bad char =\"" + val + "\"");
		}

		// Determine next state (reporting aggregate "token" if error is hit).
		state = ScanTable::getNextState(state, inChar);
		if (state == SCAN_ERROR) {
			MsgLog::logMsg(MSG_ERR, lineNum, "Bad token \"" + buffer + "\"");
		}

		// Handle bookkeeping items of the scanner.
		if (state == SCAN_START) {buffer.clear();}	// Trim unwanted chars
		if (inChar == '\n') {lineNum++;} 			// Update line count

		// Create token once an end state has been reached.
		if (state >= SCAN_END_MIN) {
			// Undo pop if token excludes this character.
			if (state >= SCAN_EXCLUDE_MIN) {
				buffer.pop_back();					// Remove from buffer
				ungetc(inChar, fptr);				// Push back to file
				if (inChar == '\n') {lineNum--;}	// Correct counter
			}

			// Create/record new token (throw out comments).
			if (state != SCAN_COMMENT) {
				ScanToken newTkn((ScanTableStates)state, lineNum, buffer);
				retTkns.push_back(newTkn);

				MsgLog::logMsg(MSG_INFO, "Found " + newTkn.toString());
			}

			// Reset "state machine".
			state = SCAN_START;
			buffer.clear();
		}

	} while(inChar != EOF);

	// Finished scanning- close the source file.
	fclose(fptr);

	string numTkns = to_string(retTkns.size());
	MsgLog::logMsg(MSG_INFO, "Scan completed: " + numTkns + " tokens found");

	// Return scanned tokens.
	return retTkns;
}

// TODO
int main(int argc, char* argv[]) {
	// Long-term arguments.
	bool optFlag = false; // defaults to "optimizations off"
	bool asmFlag = false; // defaults to "no asm file made"
	string cFilename;

	// Parse arguments.
	if (argc <= 1) {MsgLog::logMsg(MSG_ERR, "Filename not provided");}
	if (string(argv[1]) == "-h") {printInfo();}	// corner case: quaidcc.exe -h
	for (int i = 1; i < (argc - 1); i++) { 		// skip program and file names
		// Match/set flags (repeats allowed, but not unknown flags).
		string flag(argv[i]);
		if 		(flag == "-h") 		{printInfo();}
		else if (flag == "-O1") 	{optFlag = true;}
		else if (flag == "-s")		{asmFlag = true;}
		else if (flag == "-v")	   	{MsgLog::doVerbose(true);}
		else if (flag == "-Werror") {MsgLog::doWerror(true);}
		else {MsgLog::logMsg(MSG_ERR, "\"" + flag + "\" is not a valid flag");}
	}
	cFilename = argv[argc - 1];

	MsgLog::logMsg(MSG_INFO, "C Filename: " + cFilename);
	MsgLog::logMsg(MSG_INFO, string("optFlag = ") + ((optFlag) ? "true" : "false"));
	MsgLog::logMsg(MSG_INFO, string("asmFlag = ") + ((asmFlag) ? "true" : "false"));

	// Scan and tokenize the file.
	vector<ScanToken> scanTkns = scanFile(cFilename);

	return 0;
}
