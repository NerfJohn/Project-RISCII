/*
 * Parser.cpp: Functions for language specific parsing rules/support.
 */

#include <vector>
#include "Device/Terminate.h"

#include "Device/Parser.h"

using namespace std;

//==============================================================================

// Definitions for easing construction of "breakdown" sequences.
#define LIST(x)    static vector<ParseState_e> const x
#define ITEM(x)    ((ParseState_e)(x))

// Definitions for easing reading parse conditions.
#define IS(x)      if ((x) == token)
#define ELSE       if (true)

// Definitions for easing reading parse replacements.
#define ONLY(x)    {nextStates = &(x); doAppend = false; break;}
#define WITH(x)     {nextStates = &(x); doAppend = true;  break;}

//==============================================================================

// Constants containing replacements for parsed states.
LIST(SEQ_EPSILON) = {};
LIST(SEQ_DATA)    = {ITEM(PARSE_FILE),
		             ITEM(ACTION_DATA),
					 ITEM(PARSE_INIT_DATA)};
LIST(SEQ_MOD)     = {ITEM(PARSE_FILE),
		             ITEM(ACTION_MOD),
					 ITEM(TOKEN_LABEL)};
LIST(SEQ_DECL)    = {ITEM(PARSE_FILE),
		             ITEM(ACTION_DECL),
					 ITEM(TOKEN_COLON)};
LIST(SEQ_SHR)     = {ITEM(PARSE_FILE),
					 ITEM(ACTION_INSTR),
		             ITEM(PARSE_REG_IMM),
					 ITEM(TOKEN_REGISTER),
					 ITEM(TOKEN_REGISTER),
					 ITEM(PARSE_OPT_FLAGS)};

//==============================================================================
// Returns the given state as a LexToken. Returns TOKEN_INVALID if not a token.
LexToken_e Parser_asLexToken(ParseState_e const state) {
	// Returned token- GUILTY till innocent.
	LexToken_e retTkn = TOKEN_INVALID;

	// Recast to token if within range.
	LexToken_e asTkn = (LexToken_e)(state);
	if ((TOKEN_LEX_MIN <= asTkn) && (asTkn <= TOKEN_LEX_MAX)) {retTkn = asTkn;}

	// Return result of cast.
	return retTkn;
}

//==============================================================================
// Returns the given state as a ParseAction. Returns ACTION_INVALID on failure.
ParseAction_e Parser_asParseAction(ParseState_e const state) {
	// Returned action- GUILTY till innocent.
	ParseAction_e retAct = ACTION_INVALID;

	// Recast to action if within range.
	ParseAction_e asAct = (ParseAction_e)(state);
	if ((ACTION_PARSE_MIN <= asAct) && (asAct <= ACTION_PARSE_MAX)) {
		retAct = asAct;
	}

	// Return result of cast.
	return retAct;
}

//==============================================================================
// Parses top state of stack into its components for given token input.
RetErr_e Parser_parse(std::stack<ParseState_e>& stack, LexToken_e const token) {
	// GUILTY till innocent.
	RetErr_e res = RET_ERR_ERROR;

	// Prep to find suitable replacement sequence.
	vector<ParseState_e> const* nextStates = nullptr;
	bool                        doAppend   = false;

	// Empty stack? likely a compiler bug.
	if (stack.empty()) {
		Terminate_assert("Parsed with empty stack");
	}
	else {
		// Otherwise- find replacement sequence.
		switch(stack.top()) {
			case PARSE_FILE:                               // (start of parsing)
				IS(TOKEN_EOF)       WITH(SEQ_EPSILON)      // end of parsing
				IS(TOKEN_LABEL)     WITH(SEQ_DECL)
				IS(TOKEN_KW_SHR)    WITH(SEQ_SHR)
				IS(TOKEN_KW_DATA)   WITH(SEQ_DATA)
				IS(TOKEN_KW_GLOBAL) WITH(SEQ_MOD)
				break;
			case PARSE_OPT_FLAGS:
				IS(TOKEN_FLAGS)     WITH(SEQ_EPSILON)
				IS(TOKEN_REGISTER)  ONLY(SEQ_EPSILON)      // resolve to nothing
				break;
			case PARSE_REG_IMM:
				IS(TOKEN_REGISTER)  WITH(SEQ_EPSILON)
				IS(TOKEN_IMMEDIATE) WITH(SEQ_EPSILON)
				break;
			case PARSE_INIT_DATA:
				IS(TOKEN_IMMEDIATE) WITH(SEQ_EPSILON)
				IS(TOKEN_LABEL)     WITH(SEQ_EPSILON)
				break;
			default:
				// Top can't be broken down? compiler bug.
				Terminate_assert("Parsed with invalid top state");
				break;
		}
	}

	// Replace the top state as applicable.
	if (nextStates != nullptr) {
		// Get rid of state being replaced.
		stack.pop();

		// Push new sequence onto stack.
		for (ParseState_e state : *nextStates) {stack.push(state);}

		// Push instigating token (as applicable).
		if (doAppend) {stack.push((ParseState_e)(token));}

		// Successful "replacement"- innocent.
		res = RET_ERR_NONE;
	}

	// Return result of the process.
	return res;
}

