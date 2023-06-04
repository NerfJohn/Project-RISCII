/*
 * NodeAbstracts.cpp
 */

/*
 * Note:
 * This is a TEMPORARY file to hold any abstract/interface classes related to
 * the Abstract Syntax Tree (AST) as the compiler is implemented. Classes should
 * be removed and given their own files as appropriate over time.
 *
 * Should 2+ of the classes be too small to warrant their own files (or it makes
 * better sense to keep the classes together), this file should be renamed to
 * better reflect the purpose of the file.
 */

#include "MsgLog.h"

#include "NodeAbstracts.h"

using namespace std;

//////////////////////
// === IASTNode === //
//////////////////////

// TODO
std::string IASTNode::nodeToString(ParseActions node) {
	// String-ify the parse action/node specifier.
	switch (node) {
		case PARSE_PRGM_NODE: return "PRGM_NODE"; break;
		case PARSE_VDECL_NODE: return "VAR_DECL_NODE"; break;
		case PARSE_FDEF_NODE: return "FUNC_DEF_NODE"; break;
		case PARSE_ID_NODE: return "ID_NODE"; break;
		case PARSE_IF_NODE: return "IF_NODE"; break;
		case PARSE_WHILE_NODE: return "WHILE_NODE"; break;
		case PARSE_RETURN_NODE: return "RETURN_NODE"; break;
		case PARSE_ASSIGN_NODE: return "ASSIGN_NODE"; break;
		case PARSE_CALL_NODE: return "CALL_NODE"; break;
		case PARSE_LNOT_NODE: return "LOGICAL_NOT_NODE"; break;
		case PARSE_BNOT_NODE: return "BOOLEAN_NOT_NODE"; break;
		case PARSE_MINUS_NODE: return "MINUS_NODE"; break;
		case PARSE_LIT_NODE: return "LIT_NODE"; break;
		case PARSE_PLUS_NODE: return "PLUS_NODE"; break;
		case PARSE_LSHIFT_NODE: return "LEFT_SHIFT_NODE"; break;
		case PARSE_RSHIFT_NODE: return "RIGHT_SHIFT_NODE"; break;
		case PARSE_GRT_NODE: return "GRT_NODE"; break;
		case PARSE_LT_NODE: return "LT_NODE"; break;
		case PARSE_GEQ_NODE: return "GEQ_NODE"; break;
		case PARSE_LEQ_NODE: return "LEQ_NODE"; break;
		case PARSE_EQ_NODE: return "EQ_NODE"; break;
		case PARSE_NEQ_NODE: return "NEQ_NODE"; break;
		case PARSE_AND_NODE: return "AND_NODE"; break;
		case PARSE_XOR_NODE: return "XOR_NODE"; break;
		case PARSE_OR_NODE: return "OR_NODE"; break;
		default:
			// Implies node/type is corrupted- assert source code is faulty.
			string msg("IASTNode::nodeToString() called when node = ");
			MsgLog::logASSERT(msg + to_string((int)(node)));
	}

	// (Not reached, but to appease the compiler...)
	return "";
}

//////////////////////
// === IExpNode === //
//////////////////////

// TODO
int IExpNode::initFromStack(std::stack<IBuildItem*>* buildStack) {
	// Process tokens related to right hand expression.
	IBuildItem* item;
	int parenCnt = 0;
	while (buildStack->size()) {
		// Pop next item off the stack.
		item = buildStack->top();
		buildStack->pop();

		// Account for parens (before leaving the loop).
		uint8_t type = item->getBuildType();
		if (type == SCAN_RPAREN) {parenCnt++;}
		else if ((type == SCAN_LPAREN) && (parenCnt > 0)) {
			parenCnt--;
			delete item;
			continue;
		}

		// Save expression when found (continue to skip deletion).
		IExpNode* expPtr = dynamic_cast<IExpNode*>(item);
		if (expPtr != nullptr) {m_rhs = expPtr; continue;}

		// Exit based on parens (as exp capture uses continue).
		if (parenCnt == 0) {break;}
		else {delete item;} // Delete paren tokens
	}

	// Use operator to save the line number (and then throw away).
	int retNum = item->getLineNum();
	delete item;

	// Process tokens related to left hand expression (should they exist).
	parenCnt = 0;
	while (buildStack->size()) {
		// Pop next item off the stack.
		item = buildStack->top();
		buildStack->pop();

		// Account for parens (before leaving the loop).
		uint8_t type = item->getBuildType();
		if (type == SCAN_RPAREN) {parenCnt++;}
		else if ((type == SCAN_LPAREN) && (parenCnt > 0)) {
			parenCnt--;
			delete item;
			continue;
		}

		// Save expression when found (continue to skip deletion).
		IExpNode* expPtr = dynamic_cast<IExpNode*>(item);
		if (expPtr != nullptr) {m_lhs = expPtr; continue;}

		// Exit based on parens (as exp capture uses continue).
		if (parenCnt == 0) {break;}
		else {delete item;} // Delete paren tokens
	}

	// Push back last token- it wasn't related.
	buildStack->push(item);

	return retNum;
}

// TODO
std::string IExpNode::toExpString(uint8_t type) {
	// Prepare string (with punctuation).
	string tknStr = "{";

	// Add node type using conversion function.
	tknStr += IASTNode::nodeToString((ParseActions)type);

	// Add lhs info.
	tknStr += " for ";
	if (m_lhs == nullptr) {tknStr += "---";}
	else {
		if (m_lhs->isUnit()) {tknStr += "UNIT_EXPR";}
		else if (m_lhs->isUnary()) {tknStr += "UNARY_EXPR";}
		else {tknStr += "EXPR";}
	}

	// Add rhs info.
	tknStr += " and ";
	if (m_rhs == nullptr) {tknStr += "---";}
	else {
		if (m_rhs->isUnit()) {tknStr += "UNIT_EXPR";}
		else if (m_rhs->isUnary()) {tknStr += "UNARY_EXPR";}
		else {tknStr += "EXPR";}
	}

	// Finish punctuation and return.
	return tknStr + "}";
}

// TODO
void IExpNode::checkSemantics(SymbolTable* symTable,
							  std::vector<Symbol*>* symList) {
	// Pass to children (as available).
	if (m_lhs != nullptr) {m_lhs->checkSemantics(symTable, symList);}
	if (m_rhs != nullptr) {m_rhs->checkSemantics(symTable, symList);}
}
