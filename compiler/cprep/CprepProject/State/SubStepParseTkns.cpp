/*
 * SubStepParseTkns.cpp: Substep to parse tokens into series of tokens.
 */

#include "Device/Parser.h"
#include "Device/Print.h"
#include "Device/Terminate.h"
#include "Ds/FileNode.h"
#include "Ds/IncNode.h"
#include "Ds/TextNode.h"
#include "Util/ModelUtil.h"

#include "State/SubStepParseTkns.h"

using namespace std;

//==============================================================================
// Helper function to create specific nodes based off given action.
static void SubStepParseTkns_buildNode(stack<ABuildItem*>& itemStack,
		                               ParseAction_e const  action) {
	// Create the referenced node.
	ABuildItem* node = nullptr;
	switch (action) {
		case ACTION_FILE:     node = new FileNode(itemStack); break;
		case ACTION_INCLUDE:  node = new IncNode(itemStack); break;
		case ACTION_TEXT:     node = new TextNode(itemStack); break;
		default:
			// No matching node? compiler bug.
			Terminate_assert("Tried creating unknown node");
	}

	// (Ensure object was actually made.)
	if (node == nullptr) {Terminate_assert("Couldn't create node");}

	// Add back to stack for incorporating into tree.
	itemStack.push(node);
}

//==============================================================================
// TODO
// Executes sub-process to parse tokens into an ordered series of nodes.
void SubStepParseTkns_execute(DataModel_t&            model,
		                      std::queue<ItemToken*>& tkns,
						      AAstNode*&              root) {
	// (Details for debug summary at end.)
	string   fname    = "";
	uint32_t numNodes = 0;


	// Prepare stacks for grammar checking + data structure construction.
	stack<ABuildItem*>            actStack;
	stack<ParseState_e>           parseStack;
	parseStack.push((ParseState_e)(TOKEN_EOF)); // starting parse states
	parseStack.push(PARSE_FILE);

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
		ItemToken*            curItem   = tkns.front();

		// Create a new node if the "state" is actually an action (1/3 IF).
		if (curAction != ACTION_INVALID) {
			// Create the node in question (on/within stack).
			SubStepParseTkns_buildNode(actStack, curAction);
			numNodes++;

			// Pop executed action.
			parseStack.pop();
		}

		// Match tokens if "state" is actually a lex token (2/3 ELSE IF).
		else if (curTkn != TOKEN_INVALID) {
			// Ensure next token is expected.
			if (curTkn != curItem->m_lexTkn) {
				string errStr = string("Mismatched token '") +
						        curItem->m_rawStr            +
								"'";
				Print::inst().log(LOG_ERROR,
						          curItem->m_file,
								  curItem->m_line,
								  errStr);
				ModelUtil_recordError(model, RET_MIS_TKN);
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
			RetErr_e retErr = Parser_parse(parseStack, curItem->m_lexTkn);

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
		if (model.m_numErrs > 0) {break;}
	}

	// (Sanity check post parsing).
	if (model.m_numErrs == 0) {
		// Basic checks of stacks.
		if (tkns.size() > 0)      {Terminate_assert("Tokens left unparsed");}
		if (actStack.size() != 1) {Terminate_assert("Root node not found");}

		// Basic check/assigning of root.
		root = dynamic_cast<AAstNode*>(actStack.top());
		IF_NULL(root, "Root not expected type");
	}

	// (Final debug report on parsing.)
	string dbgStr = to_string(numNodes) + " nodes created";
	Print::inst().log(LOG_DEBUG, fname, dbgStr);
}
