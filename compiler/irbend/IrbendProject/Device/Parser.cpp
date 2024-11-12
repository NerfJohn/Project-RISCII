/*
 * Parser.cpp: Functions for language specific parsing rules/support.
 */

#include "Device/Parser.h"

using namespace std;

//==============================================================================
// Returns the given state as a LexToken. Returns TOKEN_INVALID if not a token.
LexToken_e Parser_asLexToken(ParseState_e const state) {
	// TODO
	return TOKEN_INVALID;
}

//==============================================================================
// Returns the given state as a ParseAction. Returns ACTION_INVALID on failure.
ParseAction_e Parser_asParseAction(ParseState_e const state) {
	// TODO
	return ACTION_INVALID;
}

//==============================================================================
// Parses top state of stack into its components for given token input.
RetErr_e Parser_parse(std::stack<ParseState_e>& stack, LexToken_e const token) {
	// TODO
	return RET_ERR_ERROR;
}

