/*
 * SubStepParseTkns.cpp: Substep to parse tokens into series of tokens.
 */

#include "Device/Parser.h"
#include "Device/Print.h"
#include "Device/Terminate.h"
#include "Domain/BuildStack_t.h"
#include "Ds/InstructionNode.h"
#include "Util/ModelUtil.h"

#include "State/SubStepParseTkns.h"

using namespace std;

//==============================================================================
// Helper function to create specific nodes based off given action.
static void SubStepParseTkns_buildNode(shared_ptr<AAsmNode>& node,
		                               BuildStack_t& itemStack,
		                               ParseAction_e const action) {
	// Create the referenced node.
	switch (action) {
		case ACTION_INSTR:
			node = move(shared_ptr<AAsmNode>(new InstructionNode(itemStack)));
			break;
		default:
			// No matching node? compiler bug.
			Terminate_assert("Tried creating unknown node");
	}

	// (Ensure object was actually made.)
	if (node == nullptr) {Terminate_assert("Couldn't create node");}
}

//==============================================================================
// Executes sub-process to parse tokens into an ordered series of nodes.
RetErr_e SubStepParseTkns_execute(DataModel_t& model,
		                          TokenList_t& tkns,
								  NodeList_t& nodes) {
	// Result of the process.
	RetErr_e retErr = RET_ERR_NONE; // INNOCENT till guilty

	// (filename for debug summary at end.)
	string fname;

	// Prepare stacks for grammar checking + data structure construction.
	BuildStack_t                  actStack;
	stack<ParseState_e>           parseStack;
	parseStack.push(PARSE_FILE);              // starting parse state

	// "Parsing till we drop".
	while(parseStack.size() > 0) {
		// (Sanity check before de-ref.)
		if (tkns.size() == 0)        {Terminate_assert("Ran out of tokens");}
		if (tkns.front() == nullptr) {Terminate_assert("NULL token parsed");}
		fname = tkns.front()->m_file; // (get filename for debug summary)

		// Analyze the next state.
		ParseState_e          curState  = parseStack.top();
		ParseAction_e         curAction = Parser_asParseAction(curState);
		LexToken_e            curTkn    = Parser_asLexToken(curState);
		shared_ptr<ItemToken> curItem   = tkns.front();

		// Create a new node if the "state" is actually an action (1/3 IF).
		if (curAction != ACTION_INVALID) {
			// Create the node in question.
			shared_ptr<AAsmNode> newNode;
			SubStepParseTkns_buildNode(newNode, actStack, curAction);

			// Append node to the list (asmld's "core data structure").
			nodes.push_back(move(newNode)); // move- we're done with it here

			// Pop executed action.
			parseStack.pop();
		}

		// Match tokens if "state" is actually a lex token (2/3 ELSE IF).
		else if (curTkn != TOKEN_INVALID) {
			// Ensure next token is expected.
			if (curTkn != curItem->m_lexTkn) {
				// Record error globally.
				string errStr = string("Mismatched token '") +
						        curItem->m_rawStr            +
								"'";
				Print::inst().log(LOG_ERROR,
						          curItem->m_file,
								  curItem->m_line,
								  errStr);
				ModelUtil_recordError(model, RET_MIS_TKN);

				// Record error locally.
				retErr = RET_ERR_ERROR;
			}

			// Move (used) tokens into action stack for construction.
			if (curItem->m_lexTkn != TOKEN_EOF) {
				actStack.push(move(curItem)); // move- we're done with it here
			}

			// Discard expended tokens.
			parseStack.pop();
			tkns.pop();
		}

		// Otherwise, break down the state into known tokens (3/3 ELSE).
		else {
			// Break down the non-terminal state.
			retErr = Parser_parse(parseStack, curItem->m_lexTkn);

			// Given token isn't correct if unable to break down state.
			if (retErr) {
				// Record error globally.
				string errStr = string("Unexpected '") +
						        curItem->m_rawStr      +
								"'";
				Print::inst().log(LOG_ERROR,
						          curItem->m_file,
								  curItem->m_line,
								  errStr);
				ModelUtil_recordError(model, RET_UEX_TKN);
			}
		}

		// If error was hit, break out to return.
		if (retErr) {break;}
	}

	// (Sanity check post parsing).
	if (retErr == RET_ERR_NONE) {
		if (tkns.size() > 0)     {Terminate_assert("Tokens left unparsed");}
		if (actStack.size() > 0) {Terminate_assert("Artifacts left unparsed");}
	}

	// (Final debug report on parsing.)
	string dbgStr = to_string(nodes.size()) + " nodes created";
	Print::inst().log(LOG_DEBUG, fname, dbgStr);

	// Return result of the process.
	return retErr;
}
