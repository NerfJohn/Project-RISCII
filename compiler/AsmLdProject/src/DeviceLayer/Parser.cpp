/*
 * Parser.cpp
 *
 * "Device to handle parsing of a stream of scan tokens into build items"
 */

#include "DeviceLayer/Printer.h"
#include "DeviceLayer/Terminate.h"
#include "Items/InstructionItem.h"
#include "Utils/ErrorUtils.h"
#include "Utils/ParseUtils.h"

#include "DeviceLayer/Parser.h"

using namespace std;

//==============================================================================
// Getter for singleton instance. Instantiates on first call.
Parser* Parser::getInst(void) {
	// Static instance- implicitly creates one (and only one) instance.
	static Parser instance;

	// Return pointer to instance.
	return &instance;
}

//==============================================================================
//
std::vector<IBuildItem*>* Parser::parseTokens(DataModel_t* model,
			                                  std::queue<ScanToken_t*>* tokens
								             ) {
	// List of items- to return (and pass ownership of).
	vector<IBuildItem*>* retItems = new vector<IBuildItem*>();

	// Prepare key data structures for creating items.
	queue<ScanToken_t*> buildTkns;
	stack<ParseState_e> parseStk;
	parseStk.push(PARSE_START);

	// Parse all the discovered tokens.
	while (!tokens->empty()) {
		// Get top level state to parse.
		ParseState_e state = parseStk.top();

		// Take the appropriate procedure based on state.
		ParseAction_e newAction = ParseUtils_asParseAction(state);
		LexToken_e    expToken  = ParseUtils_asLexToken(state);
		if (newAction != ACTION_INVALID) {
			// Ready to build item- create the item.
			IBuildItem* newItem;
			switch (newAction) {
				case ACTION_BUILD_INSTRUCTION:
					newItem = new InstructionItem(&buildTkns);
					break;
				default:
					// Unexpected action- design flaw. Assert!
					string assertStr = "called to build nonexistent item";
					Printer::getInst()->printAssert(assertStr);
					Terminate::getInst()->exit(REASON_ASSERT);
			}

			// Add item to return list.
			retItems->push_back(newItem);

			// Action completed from parse stack.
			parseStk.pop();

			// Sanity check- item took all the scan tokens, right?
			if (!buildTkns.empty()) {
				// That's not supposed to happen- assert!
				string assertStr = "build item didn't take tokens";
				Printer::getInst()->printAssert(assertStr);
				Terminate::getInst()->exit(REASON_ASSERT);
			}
		}
		else if (expToken != TOKEN_INVALID) {
			// Lexed token- check that it matches actual token.
			if (expToken == tokens->front()->m_lexTkn) {
				// Matches- move to buffer for item creation.
				buildTkns.push(tokens->front());
				tokens->pop();

				// Also clear matched token from parse stack.
				parseStk.pop();
			}
			else {
				// No match- report fatal error.
				ScanToken_t* tkn = tokens->front();
				string errStr = string("Mismatch- did not expect '")
						        + tkn->m_rawStr
								+ "' token";
				Printer::getInst()->log(LOG_ERR,
						                tkn->m_orignFile,
										tkn->m_originLine,
										errStr
									   );
				ErrorUtils_includeReason(model, REASON_EXP_PARSE);

				// Fatal error- return nullptr to indicate failure.
				return nullptr;
			}
		}
		else {
			// Sub-state- attempt to break down based on next token.
			if (!ParseUtils_parseTop(&parseStk, tokens->front()->m_lexTkn)) {
				// Token unexpected by parsing/breakdown- report fatal error.
				ScanToken_t* tkn = tokens->front();
				string errStr = string("Unexpected '")
						        + tkn->m_rawStr
								+ "'";
				Printer::getInst()->log(LOG_ERR,
						                tkn->m_orignFile,
										tkn->m_originLine,
										errStr
									   );
				ErrorUtils_includeReason(model, REASON_UNEXP_PARSE);

				// Fatal error- return nullptr to indicate failure.
				return nullptr;
			}
		}
	}

	// Sanity check- parse stack is empty/worked over, right?
	if (!parseStk.empty()) {
		// Scanner/Parser should work to ensure proper reading- assert!
		string assertStr = "parse stack left in non-empty state";
		Printer::getInst()->printAssert(assertStr);
		Terminate::getInst()->exit(REASON_ASSERT);
	}

	// (Inform debugging users).
	string dbgStr = to_string(retItems->size()) + " items created";
	Printer::getInst()->log(LOG_DEBUG, dbgStr);

	// Successfully finished parsing- return items (and pointer ownership).
	return retItems;
}
