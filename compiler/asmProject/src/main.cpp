/*
 * main.cpp
 *
 *  Created on: Jan 1, 2024
 *      Author: John
 */


#include <iostream>
#include <stack>
#include <queue>
#include "AsmToken.h"
#include "SyntaxToken_e.h"
#include "ScanState.h"
#include "ParseState.h"

using namespace std;


// TODO- example of scanning file input into scanned/parsed data structure.
void syntaxMain(int argc, char* argv[]) {
	cout << "Running Scanner Example" << endl;

	// Just get a file input quick...
	if (argc < 2) {cout << "Please provide file" << endl; return;}
	FILE* fptr = fopen(argv[1], "r");
	if (!fptr) {cout << "Failed to open file" << endl; return ;}

	// General Scanning Loop.
	string buffer;
	ScanState state;
	queue<AsmToken*> tokens;
	int lineNum = 1;
	do {
		// Reset per new start to tokenizing.
		buffer = "";
		state.reset();
		uint8_t peekChar = fgetc(fptr); ungetc(peekChar, fptr);
		state.nextState(peekChar);
		while(state.asType() != SCAN_TYPE_TOKEN) {
			// Report error in scanning.
			if (state.asType() == SCAN_TYPE_ERROR) {
				cout << "ERROR (line " << lineNum << "): buf=" << buffer
						<< ", peek=" << fgetc(fptr) << endl;
				return;
			}

			// Pop char, adding it to the buffer if part of a token.
			uint8_t popChar = fgetc(fptr);
			if (state.asType() == SCAN_TYPE_SCANNING) {
				buffer += popChar;
			}

			// Tracking line number.
			if (popChar == '\n') {
				lineNum++;
			}

			// Get next state with peek.
			uint8_t peekChar = fgetc(fptr); ungetc(peekChar, fptr);
			state.nextState(peekChar);
		}

		// Record token (except comments).
		cout << "TOKEN (line" << lineNum << "): buf=" << buffer
				<< ", tkn=" << state.asToken() << endl;
		if (state.asToken() != TOKEN_COMMENT) {
			AsmToken* newTkn = new AsmToken(state.asToken(), buffer);
			tokens.push(newTkn);
		}
	} while(state.asToken() != TOKEN_EOF);

	// General parsing loop.
	ParseState parser;
	stack<SyntaxToken_e> actStack;
	parser.reset();
	while((parser.asType() != PARSE_TYPE_EMPTY) && tokens.size()) {
		// Advance next state.
		SyntaxToken_e tkn = tokens.front()->getType(); tokens.pop();
		actStack.push(tkn);
		parser.nextState(tkn);

		// Respond to errors.
		if (parser.asType() == PARSE_TYPE_NO_MATCH) {
			cout << "NO MATCH: tkn=" << tkn << endl;
			return;
		}
		else if (parser.asType() == PARSE_TYPE_MISMATCH) {
			SyntaxToken_e other = parser.asToken();
			cout << "MISMATCH: exp=" << other << ", act=" << tkn << endl;
			return;
		}

		// Do actions.
		while (parser.asType() == PARSE_TYPE_ACTION) {
			cout << "ACTION: action=" << parser.popAction() << endl;
		}
	}
	if (parser.asType() != PARSE_TYPE_EMPTY) {
		cout << "ERROR: Parser wasn't cleared by end of tokens" << endl;
		return;
	}
	else if (tokens.size()) {
		cout << "ERROR: Extra tokens left over from parsing" << endl;
		return;
	}
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
	syntaxMain(argc, argv);

	// Not really intended to hit- may as well return "success".
	return 0;
}

