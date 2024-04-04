/*
 * ParseUtils.cpp
 *
 *  Created on: Apr 1, 2024
 *      Author: John
 */

#include <iostream>
#include <stack>
#include "../Parsing/AsmToken.h"

#include "ControlItem.h"
#include "DataItem.h"
#include "FuncItem.h"
#include "InstrItem.h"
#include "LabelItem.h"

#include "../Parsing/ParseState.h" // Auto-generated file (include ONCE)
#include "../Parsing/ScanState.h"  // Auto-generated file (include ONCE)

#include "../Parsing/ParseUtils.h"

using namespace std;

// TODO- HELPER: Opens read file- fatally reporting inability to do so. Can fatal error.
FILE* ParseUtils_openReadFile(std::string fName) {
	// Attempt to open file for reading.
	FILE* fptr = fopen(fName.c_str(), "r");

	// Fatal error if unable to open.
	if (!fptr) {
		cout << "FATAL ERR: Cannot open " << fName << endl;
		exit(1);
	}

	// Opened successfully- return file pointer.
	return fptr;
}

// TODO- HELPER: Closes file (if open).
void ParseUtils_closeFile(FILE* fptr) {
	// Assume valid pointer == open file (for now...).
	fclose(fptr);
}

// TODO- Lex file into vector of assembler defined tokens. Can fatal error.
std::queue<AsmToken*> ParseUtils_lexFile(std::string fName) {
	// Open the file.
	FILE* fptr = ParseUtils_openReadFile(fName);

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
				cout << "FATAL ERR (" << fName << ":" << lineNum << "): ";
				cout << "Unknown token (Buffer=\"" << buffer << "\", ";
				cout << "peek='" << (char)fgetc(fptr) << "')" << endl;

				exit(1);
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
		string origin = fName + ":" + to_string(lineNum);
		cout << "INFO (" << origin << "): Found Token ";
		cout << "(Buffer=\"" << buffer << "\", type(#)=";
		cout << state.asToken() << ")" << endl;
		if (state.asToken() != TOKEN_COMMENT) {
			// Create/save new token.
			AsmToken* newTkn = new AsmToken(state.asToken(), buffer, origin);
			tokens.push(newTkn);
		}
	} while(state.asToken() != TOKEN_EOF);

	// All done with file- close for posterity.
	ParseUtils_closeFile(fptr);

	// Return file's lex-ed output to caller.
	return tokens;
}

// TODO- Parses token vector into data structure for analysis. Can fatal error.
std::vector<ABuildItem*> ParseUtils_parseTokens(std::queue<AsmToken*> tokens) {
	// General parsing loop.
	ParseState parser;
	stack<AsmToken*> actStack;
	vector<ABuildItem*> mainDS;
	parser.reset();
	while((parser.asType() != PARSE_TYPE_EMPTY) && tokens.size()) {
		// Advance next state.
		actStack.push(tokens.front());
		AsmToken* tkn = tokens.front(); tokens.pop();
		parser.nextState(tkn->getType());

		// Respond to errors.
		if (parser.asType() == PARSE_TYPE_NO_MATCH) {
			cout << "FATAL ERR (" << tkn->getOrigin() << "): ";
			cout << "Unexpected (Tkn=" << tkn->asStr() << ")" << endl;

			exit(1);
		}
		else if (parser.asType() == PARSE_TYPE_MISMATCH) {
			SyntaxToken_e other = parser.asToken();
			cout << "FATAL ERR (" << tkn->getOrigin() << "): ";
			cout << "Expected (Exp type=" << other << ", ";
			cout << "Tkn=" << tkn->asStr() << ")" << endl;

			exit(1);
		}

		// Do actions.
		while (parser.asType() == PARSE_TYPE_ACTION) {
			// Use action to create item.
			ParseAction_e act = parser.popAction();
			ABuildItem* newItem;
			switch(act) {
				case ACTION_INSTR:
					newItem = new InstrItem(actStack); break;
				case ACTION_DATA:
					newItem = new DataItem(actStack); break;
				case ACTION_FUNCTION:
					newItem = new FuncItem(actStack); break;
				case ACTION_LABEL:
					newItem = new LabelItem(actStack); break;
				case ACTION_DIRECTIVE:
					newItem = new ControlItem(actStack); break;
				default:
					cout << "FATAL ERR: (" << actStack.top()->getOrigin() << "): ";
					cout << "Bad Action (act(#)=" << act << ")" << endl;

					exit(1);
			}

			// (For debugging) report new item.
			cout << "INFO (" << newItem->getOrigin() << "): ";
			cout << "New Item (item=" << newItem->asStr() << ")" << endl;

			// Save to main data structure.
			mainDS.push_back(newItem);
		}
	}
	if (parser.asType() != PARSE_TYPE_EMPTY) {
		cout << "FATAL ERR: Parser not empty" << endl;
		exit(1);
	}
	else if (tokens.size()) {
		cout << "FATAL ERR: Tokens not fully parsed" << endl;
		exit(1);
	}

	// Return parsed data structure.
	return mainDS;
}
