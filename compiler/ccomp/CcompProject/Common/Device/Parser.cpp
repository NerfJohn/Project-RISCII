/*
 * Parser.cpp: Iteratively parses lex tokens.
 */

#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Common/Util/ParserDefs.h"

#include "Common/Device/Parser.h"

using namespace std;

//==============================================================================
// Constructor- preps to parse token list into actions/nodes.
Parser::Parser(std::queue<Ptr<LexToken>>& tkns) {
	// Save reference.
	m_tkns = tkns;

	// Prep parse stack.
	m_parseStack = {};
	m_parseStack.push(PARSER_START);

	// Haven't parsed yet- nothing to share.
	m_actCode  = PARSER_UNEXPECTED;
	m_actStack = {};
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
			Print_bug("parse() empty stack");
			Terminate_bug(); // does NOT return
		}
		m_actCode = m_parseStack.top();
		m_parseStack.pop();
		
		// Identify state type.
		isAction = isAct(m_actCode);
		isToken  = isTkn(m_actCode);
		
		// "Skip" processing if an action.
		if (isAction == false) {
			// Peek next token.
			if (m_tkns.size() == 0) {
				Print_bug("parse() no tokens");
				Terminate_bug(); // does NOT return
			}
			Ptr<LexToken> tkn = m_tkns.front();
			
			// Compare peeked/state tokens, or break down nonterminal.
			if (isToken) {
				if (m_actCode != tkn->m_type) {m_actCode = PARSER_MISMATCH;}
				m_actStack.push(tkn.toType<IBuildItem>()); // push to report
				m_tkns.pop();
			}
			else {
				// Parse state.
				vector<int> const* states = nullptr;
				RetErr_e res = parseState(m_actCode, tkn->m_type, states);
				
				// Push new states to parse stack (or error out).
				if (res == RET_ERR_ERROR) {
					m_actCode = PARSER_UNEXPECTED;
					m_actStack.push(tkn.toType<IBuildItem>()); // push to report
					m_tkns.pop();
				}
				else {
					if (states == nullptr) {
						Print_bug("parse() bad sequence");
						Terminate_bug();
					}
					for (int item : *states) {
						if (item == PARSER_THIS) {
							m_parseStack.push(tkn->m_type);
						}
						else {m_parseStack.push(item);}
					}
				}
			}
		}
		
		// Break out early for errors.
		if ((m_actCode == PARSER_MISMATCH) ||
			(m_actCode == PARSER_UNEXPECTED)) {
			break;
		}
	} while (isAction == false);
}
