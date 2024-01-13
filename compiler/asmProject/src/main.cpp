/*
 * main.cpp
 *
 *  Created on: Jan 1, 2024
 *      Author: John
 */


#include <iostream>
#include "SyntaxToken_e.h"
#include "ScanState.h"

using namespace std;


// TODO- example of scanning file input into tokens.
void scanMain(int argc, char* argv[]) {
	cout << "Running Scanner Example" << endl;

	// Just get a file input quick...
	if (argc < 2) {cout << "Please provide file" << endl; return;}
	FILE* fptr = fopen(argv[1], "r");
	if (!fptr) {cout << "Failed to open file" << endl; return ;}

	// General Scanning Loop.
	string buffer;
	ScanState state;
	do {
		// Reset per new start to tokenizing.
		buffer = "";
		state.reset();
		uint8_t peekChar = fgetc(fptr); ungetc(peekChar, fptr);
		state.nextState(peekChar);
		while(state.asType() != SCAN_STATE_TOKEN) {
			// Report error in scanning.
			if (state.asType() == SCAN_STATE_ERROR) {
				cout << "ERROR: buf=" << buffer << ", peek=" << fgetc(fptr) << endl;
				return;
			}

			// Pop char, adding it to the buffer if part of a token.
			uint8_t popChar = fgetc(fptr);
			if (state.asType() == SCAN_STATE_SCANNING) {
				buffer += popChar;
			}

			// Get next state with peek.
			uint8_t peekChar = fgetc(fptr); ungetc(peekChar, fptr);
			state.nextState(peekChar);
		}

		// Report parsed token.
		cout << "TOKEN: buf=" << buffer << ", tkn=" << state.asToken() << endl;
	} while(state.asToken() != TOKEN_EOF);
}

/*
 * == Main() Program ==
 * Start of the program. Defers to selected example program.
 *
 * Comment/Uncomment calls to example programs to select which one is desired in
 * the moment.
 *
 * @param argc number of arguments/strings under argv
 * @param argv array of strings/arguments in left-right call order
 * @return general code indicating result, 0 is typically a success
 */
int main(int argc, char* argv[]) {
	// Run example program.
	scanMain(argc, argv);

	// Not really intended to hit- may as well return "success".
	return 0;
}

