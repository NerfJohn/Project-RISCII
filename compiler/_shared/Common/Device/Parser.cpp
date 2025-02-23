/*
 * Parser.cpp: Iteratively parses lex tokens.
 */

#include "Common/Device/Terminate.h"

#include "Common/Device/Parser.h"

using namespace std;

//==============================================================================
// Constructor- inits parser for given input data.
Parser::Parser(ParserInit_t const& initData) {
	// Save/copy initializers.
	m_tkns            = initData.m_tkns;            // copy of list
	m_parse           = initData.m_parse;
	m_isTkn           = initData.m_isTkn;
	m_isAct           = initData.m_isAct;
	m_parseStart      = initData.m_parseStart;
	m_parseMismatch   = initData.m_parseMismatch;
	m_parseUnexpected = initData.m_parseUnexpected;
	
	// Preset internal/shared data.
	m_actCode     = m_parseStart;
	m_actStack    = {};
	m_parseStack  = {};
	m_parseStack.push(m_parseStart);
}

//==============================================================================
// Parses tokens until node/"action" can be created from act stack.
void Parser::parse(void) {
	// Parse until action (or error) is found.
	bool isToken  = false;
	bool isAction = false;
	do {
		// Pop top of parse stack,
		if (m_parseStack.size() == 0) {
			Terminate::inst().assert("parse() empty stack");
		}
		m_actCode = m_parseStack.top();
		m_parseStack.pop();
		
		// Identify state type.
		isAction = m_isAct(m_actCode);
		isToken  = m_isTkn(m_actCode);
		
		// "Skip" processing if an action.
		if (isAction == false) {
			// Peek next token.
			if (m_tkns.size() == 0) {
				Terminate::inst().assert("parse() no tokens");
			}
			LexToken* tkn = m_tkns.front();
			IF_NULL(tkn, "parse() null token");
			
			// Compare peeked/state tokens, or break down nonterminal.
			if (isToken) {
				if (m_actCode != tkn->m_type) {m_actCode = m_parseMismatch;}
				m_actStack.push(tkn); // pop over to build item regardless
				m_tkns.pop();         // error pushes it to report it
			}
			else {
				// Parse state.
				vector<int> const* states = nullptr;
				ParseOp_e res = m_parse(&states, m_actCode, tkn->m_type);
				
				// Push new states to parse stack (or error out).
				if (res == PARSE_OP_ERROR) {
					m_actCode = m_parseUnexpected;
					m_actStack.push(tkn); // pop over to build item
					m_tkns.pop();         // error pushes it to report it
				}
				else {
					IF_NULL(states, "parse() bad sequence");
					for (int item : *states) {m_parseStack.push(item);}
					if (res == PARSE_OP_INCLUDE) {
						m_parseStack.push(tkn->m_type);
					}
				}
			}
		}
		
		// Break out early for errors.
		if ((m_actCode == m_parseMismatch) || 
			(m_actCode == m_parseUnexpected)) {
			break;
		}
	} while (isAction == false);
}
