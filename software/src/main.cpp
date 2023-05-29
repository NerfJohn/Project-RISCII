/*
 * main.cpp
 */

#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <queue>
#include "MsgLog.h"
#include "ParseTable.h"
#include "ScanToken.h"
#include "ScanTable.h"

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
string actionToString(ParseActions action) {
	// Return string-ified action.
	switch (action) {
		case PARSE_PRGM_NODE: return "PRGM_NODE";
		case PARSE_VDECL_NODE: return "VDECL_NODE";
		case PARSE_FDEF_NODE: return "FDEF_NODE";
		case PARSE_ID_NODE: return "ID_NODE";
		case PARSE_IF_NODE: return "IF_NODE";
		case PARSE_WHILE_NODE: return "WHILE_NODE";
		case PARSE_RETURN_NODE: return "RETURN_NODE";
		case PARSE_ASSIGN_NODE: return "ASSIGN_NODE";
		case PARSE_CALL_NODE: return "CALL_NODE";
		case PARSE_LNOT_NODE: return "LNOT_NODE";
		case PARSE_BNOT_NODE: return "BNOT_NODE";
		case PARSE_MINUS_NODE: return "MINUS_NODE";
		case PARSE_LIT_NODE: return "LIT_NODE";
		case PARSE_PLUS_NODE: return "PLUS_NODE";
		case PARSE_LSHIFT_NODE: return "LSHIFT_NODE";
		case PARSE_RSHIFT_NODE: return "RSHIFT_NODE";
		case PARSE_GRT_NODE: return "GRT_NODE";
		case PARSE_LT_NODE: return "LT_NODE";
		case PARSE_GEQ_NODE: return "GEQ_NODE";
		case PARSE_LEQ_NODE: return "LEQ_NODE";
		case PARSE_EQ_NODE: return "EQ_NODE";
		case PARSE_NEQ_NODE: return "NEQ_NODE";
		case PARSE_AND_NODE: return "AND_NODE";
		case PARSE_XOR_NODE: return "XOR_NODE";
		case PARSE_OR_NODE: return "OR_NODE";
		default: MsgLog::logASSERT("Bad Node print");
	}
	return "";
}

// TODO
void parseTokens(queue<ScanToken> tokens) {
	// (INFO Checkpoint: Parsing started.)
	MsgLog::logINFO("Parsing...");
	int numNodes = 0;				// Info statistics

	// Stacks used to conduct parsing/building.
	stack<uint8_t> parseStack;		// for parsing tokens
	stack<ScanToken> buildStack;	// for building AST

	// Initialize parse stack.
	constexpr uint8_t EOF_TKN = 0xff; // "-1" equivalent
	parseStack.push(EOF_TKN);
	parseStack.push(PARSE_START);

	// Work until parse stack has been cleared.
	while (parseStack.size()) {
		// Get current state...
		uint8_t state = parseStack.top();
		parseStack.pop();

		// ...and current token details (or "EOF" if unavailable).
		constexpr int NO_LINE_NUM = -1;
		ScanTableStates tknType = (tokens.size()) ? tokens.front().getType() :
				(ScanTableStates)(EOF_TKN);
		int tknLineNum = (tokens.size()) ? tokens.front().getLineNum() :
				NO_LINE_NUM;

		// Handle based on "grouping" (action vs. token vs. sub-state).
		if ((PARSE_NODE_MIN <= state) && (state <= PARSE_NODE_MAX)) {
			// TODO: Perform action- just print for now.
			MsgLog::logINFO("TODO- Create " +
					actionToString((ParseActions)state));

			// Info statistics bookkeeping.
			numNodes++;
		}
		else if (((SCAN_END_MIN <= state) && (state <= SCAN_END_MAX)) ||
				  (EOF_TKN == state)) {
			// Check expected/actual token (or hit fatal error).
			if (state != tknType) {
				string exp = (EOF_TKN == state) ?
						"EOF" : ScanToken::typeToString((ScanTableStates)state);
				string act = (EOF_TKN == state) ?
						"EOF" : ScanToken::typeToString(tknType);
				MsgLog::logERR("Expected " + exp + ", found " + act,
						tknLineNum);
			}

			// Token parsed, pop from queue to build stack (as applicable).
			if (EOF_TKN != state) {
				buildStack.push(tokens.front());
				tokens.pop();
			}
		}
		else { // Otherwise, a sub-state
			// Use parse table to get new states (or hit fatal error).
			vector<uint8_t>* newStates =
					ParseTable::getNextStates(state, tknType);
			if (newStates == nullptr) {
				MsgLog::logERR("Unexpected " + ScanToken::typeToString(tknType),
							   tknLineNum);
			}

			// Push new states to parse stack (in reverse- LIFO stack).
			for (uint8_t i = newStates->size(); i > 0; i--) {
				parseStack.push(newStates->at(i - 1));
			}
		}
	}

	// (INFO Checkpoint: Parsing started.)
	string nodeCnt = to_string(numNodes);
	MsgLog::logINFO("Parse Completed- " + nodeCnt + " nodes created");
}

// TODO
queue<ScanToken> scanFile(string cFilename) {
	// (INFO Checkpoint: Scanning started.)
	MsgLog::logINFO("Scanning...");

	// Prepare file to be read character by character.
	FILE* fptr = fopen(cFilename.c_str(), "r");
	if (!fptr) {MsgLog::logERR("Failed to open \"" + cFilename + "\"");}

	// Preset important scanning variables.
	uint8_t state = SCAN_START;	// Keep track of when/what to make a token
	string buffer;				// Record scanned chars for tokens
	uint32_t lineNum = 1;		// Record lines for warnings/errors
	queue<ScanToken> retTkns;	// List of tokens scanned in

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
			MsgLog::logERR("Bad char =\"" + val + "\"", lineNum);
		}

		// Determine next state (reporting aggregate "token" if error is hit).
		state = ScanTable::getNextState(state, inChar);
		if (state == SCAN_ERROR) {
			MsgLog::logERR("Bad token \"" + buffer + "\"", lineNum);
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
				// Create/save the token.
				ScanToken newTkn((ScanTableStates)state, lineNum, buffer);
				retTkns.push(newTkn);

				// (INFO Update: New token created.)
				MsgLog::logINFO("Found " + newTkn.toString());
			}

			// Reset "state machine".
			state = SCAN_START;
			buffer.clear();
		}

	} while(inChar != EOF);

	// Finished scanning- close the source file.
	fclose(fptr);

	// (INFO Checkpoint: Scan finished.)
	string numTkns = to_string(retTkns.size());
	MsgLog::logINFO("Scan completed- " + numTkns + " tokens found");

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
	if (argc <= 1) {MsgLog::logERR("Filename not provided");}
	for (int i = 1; i < (argc - 1); i++) { 		// skip program and file names
		// Match/set flags (repeats allowed, but not unknown flags).
		string flag(argv[i]);
		if 		(flag == "-h") 		{printInfo();}
		else if (flag == "-O1") 	{optFlag = true;}
		else if (flag == "-s")		{asmFlag = true;}
		else if (flag == "-v")	   	{MsgLog::setVerbose(true);}
		else if (flag == "-Werror") {MsgLog::setWerror(true);}
		else {MsgLog::logERR("\"" + flag + "\" is not a valid flag");}
	}
	if (string(argv[argc - 1]) == "-h") {printInfo();}
	cFilename = argv[argc - 1];

	// (INFO Checkpoint: Beginning options handled.)
	MsgLog::logINFO("C Filename: " + cFilename);
	MsgLog::logINFO(string("optFlag = ") + ((optFlag) ? "true" : "false"));
	MsgLog::logINFO(string("asmFlag = ") + ((asmFlag) ? "true" : "false"));

	////////////////////////////////
	// Main Compilation Procedure //
	////////////////////////////////

	// Scan and tokenize the file.
	queue<ScanToken> scanTkns = scanFile(cFilename);

	// Parse file into Abstract Syntax Tree (AST).
	parseTokens(scanTkns);

	MsgLog::logINFO("Current end of program reached");

	return 0;
}
