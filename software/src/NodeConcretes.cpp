/*
 * NodeConcretes.cpp
 */

/*
 * Note:
 * This is a TEMPORARY file to hold any concrete classes related to the Abstract
 * Syntax Tree (AST) as the compiler is implemented. Classes should be removed
 * and given their own files as appropriate over time.
 *
 * Should 2+ of the classes be too small to warrant their own files (or it makes
 * better sense to keep the classes together), this file should be renamed to
 * better reflect the purpose of the file.
 */

#include "NodeConcretes.h"

#include "MsgLog.h"

using namespace std;

//////////////////////
// === PrgmNode === //
//////////////////////

// TODO
PrgmNode::PrgmNode(std::stack<IBuildItem*>* buildStack) {
	// Temporary buffer to hold popped decls.
	vector<IDeclNode*> buffer;

	// Pop contiguous decls until none remain.
	IDeclNode* item = dynamic_cast<IDeclNode*>(buildStack->top());
	while (item != nullptr) {
		// Pop into buffer.
		buffer.push_back(item);
		buildStack->pop();

		// Get next item (if it's even available).
		if (buildStack->size() == 0) {break;}
		item = dynamic_cast<IDeclNode*>(buildStack->top());
	}

	// Populate node's declaration list (in reverse- stack was LIFO).
	for (int i = buffer.size(); i > 0; i--) {
		m_declList.push_back(buffer.at(i - 1));
	}
}

// TODO
std::string PrgmNode::toString(void) {
	// Prepare string (with punctuation).
	string tknStr = "{";

	// Add node type using conversion function.
	tknStr += IASTNode::nodeToString(PARSE_PRGM_NODE);

	// Add number of declarations/defined units.
	tknStr += " of " + to_string(m_declList.size()) + " decls";

	// Finish punctuation and return.
	return tknStr + "}";
}

/////////////////////////
// === VarDeclNode === //
/////////////////////////

// TODO
VarDeclNode::VarDeclNode(std::stack<IBuildItem*>* buildStack) {
	// Process tokens until un-related token is found (or stack is empty).
	while (buildStack->size()) {
		// Get next token.
		IBuildItem* item = buildStack->top();

		// Process based on type.
		uint8_t type = item->getBuildType();
		if (type == SCAN_SEMICOLON) {/* Related, but no actions */}
		else if (type == SCAN_ID) {m_id = item->getValue();}
		else if ((type == SCAN_INT) || (type == SCAN_CHAR)) {
			m_varType = (type == SCAN_INT) ? TYPE_INT : TYPE_CHAR;
			m_lineNum = item->getLineNum();
		}
		else if (type == SCAN_UNSIGNED) {
			m_varType = (m_varType == TYPE_INT) ? TYPE_UINT : TYPE_UCHAR;
			m_lineNum = item->getLineNum();
		}
		else {
			// Un-related token- break out of processing loop.
			break;
		}

		// Consume/free the memory.
		buildStack->pop();
		delete item;
	}
}

// TODO
std::string VarDeclNode::toString(void) {
	// Prepare string (with punctuation).
	string tknStr = "{";

	// Add node type using conversion function.
	tknStr += IASTNode::nodeToString(PARSE_VDECL_NODE);

	// Add type.
	tknStr += " of " + VarType::toString(m_varType);

	// Add name.
	tknStr += " named \"" + m_id + "\"";

	// Finish punctuation and return.
	return tknStr + "}";
}

/////////////////////////
// === FuncDefNode === //
/////////////////////////

// TODO
FuncDefNode::FuncDefNode(std::stack<IBuildItem*>* buildStack) {
	// Temporary buffers to hold popped params/stmts.
	vector<IDeclNode*> paramBuffer;
	vector<IStmtNode*> stmtBuffer;

	// Phase 1: Process "body" tokens until un-related token is found.
	while (buildStack->size()) { // Use stack size out of uniformity
		// Get next token.
		IBuildItem* item = buildStack->top();
		buildStack->pop(); // "Inclusive" consumption of bracket.

		// Pop "stmts" into buffer.
		IStmtNode* stmtPtr = dynamic_cast<IStmtNode*>(item);
		if (stmtPtr != nullptr) {stmtBuffer.push_back(stmtPtr);}

		// Otherwise, process brackets.
		uint8_t type = item->getBuildType();
		if (type == SCAN_RCURLY) 		{/* Related, but unused */ delete item;}
		else if (type == SCAN_LCURLY) 	{/* End of body */ delete item; break;}
	}

	// Phase 2: Process "param" tokens until un-related token is found.
	while (buildStack->size()) { // Use stack size out of uniformity
		// Get next token.
		IBuildItem* item = buildStack->top();
		buildStack->pop(); // "Inclusive" consumption of parens.

		// Pop "stmts" into buffer.
		IDeclNode* declPtr = dynamic_cast<IDeclNode*>(item);
		if (declPtr != nullptr) {paramBuffer.push_back(declPtr);}

		// Otherwise, process commas/parens.
		uint8_t type = item->getBuildType();
		if (type == SCAN_COMMA) 		{/* Related, but unused */ delete item;}
		else if (type == SCAN_RPAREN) 	{/* Related, but unused */ delete item;}
		else if (type == SCAN_LPAREN) 	{/* End of body */ delete item; break;}
	}

	// Phase 3: Process type/id (similar to a variable declaration).
	while (buildStack->size()) {
		// Get next token.
		IBuildItem* item = buildStack->top();

		// Process based on type.
		uint8_t type = item->getBuildType();
		if (type == SCAN_ID) {m_id = item->getValue();}
		else if ((type == SCAN_INT) || (type == SCAN_CHAR)) {
			m_varType = (type == SCAN_INT) ? TYPE_INT : TYPE_CHAR;
			m_lineNum = item->getLineNum();
		}
		else if (type == SCAN_UNSIGNED) {
			m_varType = (m_varType == TYPE_INT) ? TYPE_UINT : TYPE_UCHAR;
			m_lineNum = item->getLineNum();
		}
		else {
			// Un-related token- break out of processing loop.
			break;
		}

		// Consume/free the memory.
		buildStack->pop();
		delete item;
	}

	// Populate node's param/stmt lists (in reverse- stack was LIFO).
	for (int i = paramBuffer.size(); i > 0; i--) {
		m_paramList.push_back(paramBuffer.at(i - 1));
	}
	for (int i = stmtBuffer.size(); i > 0; i--) {
		m_stmtList.push_back(stmtBuffer.at(i - 1));
	}
}

// TODO
std::string FuncDefNode::toString(void) {
	// Prepare string (with punctuation).
	string tknStr = "{";

	// Add node type using conversion function.
	tknStr += IASTNode::nodeToString(PARSE_FDEF_NODE);

	// Add type.
	tknStr += " of " + VarType::toString(m_varType);

	// Add name.
	tknStr += " named \"" + m_id + "\"";

	// Add param/stmt stats.
	tknStr += ": " + to_string(m_paramList.size()) + " params";
	tknStr += ", " + to_string(m_stmtList.size()) + " stmts";

	// Finish punctuation and return.
	return tknStr + "}";
}

////////////////////
// === IDNode === //
////////////////////

// TODO
IDNode::IDNode(std::stack<IBuildItem*>* buildStack) {
	// Always the top token on the stack- simple process.
	IBuildItem* item = buildStack->top();
	m_lineNum = item->getLineNum();
	m_id = item->getValue();

	// Consume/free token.
	buildStack->pop();
	delete item;
}

// TODO
std::string IDNode::toString(void) {
	// Prepare string (with punctuation).
	string tknStr = "{";

	// Add node type using conversion function.
	tknStr += IASTNode::nodeToString(PARSE_ID_NODE);

	// Add name.
	tknStr += " named \"" + m_id + "\"";

	// Finish punctuation and return.
	return tknStr + "}";
}

/////////////////////
// === LITNode === //
/////////////////////

// TODO
LITNode::LITNode(std::stack<IBuildItem*>* buildStack) {
	// Always the top token on the stack- simple process.
	IBuildItem* item = buildStack->top();
	m_lineNum = item->getLineNum();
	m_strValue = item->getValue();

	// Consume/free token.
	buildStack->pop();
	delete item;
}

// TODO
std::string LITNode::toString(void) {
	// Prepare string (with punctuation).
	string tknStr = "{";

	// Add node type using conversion function.
	tknStr += IASTNode::nodeToString(PARSE_LIT_NODE);

	// Add name.
	tknStr += " = \"" + m_strValue + "\"";

	// Finish punctuation and return.
	return tknStr + "}";
}

////////////////////////
// === AssignNode === //
////////////////////////

// TODO
AssignNode::AssignNode(std::stack<IBuildItem*>* buildStack) {
	// Process tokens until un-related token is found.
	IBuildItem* item;
	while (buildStack->size()) { // Use stack size out of uniformity
		// Get next token.
		item = buildStack->top();
		buildStack->pop();

		// Link rhs.
		IExpNode* expPtr = dynamic_cast<IExpNode*>(item);
		if (expPtr != nullptr) {m_rhs = expPtr; continue;}

		// Otherwise, process other tokens.
		if (item->getBuildType() == SCAN_ASSIGN) {/* end of expr */ break;}
		else {/* Related, but unused */ delete item;}
	}

	// Use assign operator for line number/location.
	m_lineNum = item->getLineNum();
	delete item;

	// Link identifier to assignment.
	item = buildStack->top();
	m_lhs = dynamic_cast<IDNode*>(item);
	buildStack->pop();
}

// TODO
std::string AssignNode::toString(void) {
	// Prepare string (with punctuation).
	string tknStr = "{";

	// Add node type using conversion function.
	tknStr += IASTNode::nodeToString(PARSE_ASSIGN_NODE);

	// Add lhs info.
	tknStr += " of \"" + m_lhs->getValue() + "\"";

	// Add rhs info as available.
	if (m_rhs->isUnit()) {tknStr += " using UNIT_EXPR";}
	else if (m_rhs->isUnary()) {tknStr += " using UNARY_EXPR";}

	// Finish punctuation and return.
	return tknStr + "}";
}

/////////////////////
// === RetNode === //
/////////////////////

// TODO
RetNode::RetNode(std::stack<IBuildItem*>* buildStack) {
	// Process tokens until un-related token is found.
	IBuildItem* item;
	while (buildStack->size()) { // Use stack size out of uniformity
		// Get next token.
		item = buildStack->top();
		buildStack->pop();

		// Link rhs.
		IExpNode* expPtr = dynamic_cast<IExpNode*>(item);
		if (expPtr != nullptr) {m_exp = expPtr; continue;}

		// Otherwise, process other tokens.
		if (item->getBuildType() == SCAN_RETURN) {/* end of expr */ break;}
		else {/* Related, but unused */ delete item;}
	}

	// Use return operator for line number/location.
	m_lineNum = item->getLineNum();
	delete item;
}

// TODO
std::string RetNode::toString(void) {
	// Prepare string (with punctuation).
	string tknStr = "{";

	// Add node type using conversion function.
	tknStr += IASTNode::nodeToString(PARSE_RETURN_NODE);

	// Add exp info as available.
	if (m_exp->isUnit()) {tknStr += " using UNIT_EXPR";}
	else if (m_exp->isUnary()) {tknStr += " using UNARY_EXPR";}

	// Finish punctuation and return.
	return tknStr + "}";
}

////////////////////
// === IfNode === //
////////////////////

// TODO
IfNode::IfNode(std::stack<IBuildItem*>* buildStack) {
	// Temporary buffer to hold popped stmts.
	vector<IStmtNode*> stmtBuffer;

	// Process "body" tokens until un-related token is found.
	IBuildItem* item;
	while (buildStack->size()) { // Use stack size out of uniformity
		// Get next token.
		item = buildStack->top();
		buildStack->pop();

		// Pop "stmts" into buffer.
		IStmtNode* stmtPtr = dynamic_cast<IStmtNode*>(item);
		if (stmtPtr != nullptr) {stmtBuffer.push_back(stmtPtr); continue;}

		// Otherwise, process other tokens.
		if (item->getBuildType() == SCAN_LCURLY) {/* end of expr */ break;}
		else {/* Related, but unused */ delete item;}
	}
	delete item; // Throw out left curly

	// Process "conditional" tokens until un-related token is found.
	while (buildStack->size()) { // Use stack size out of uniformity
		// Get next token.
		item = buildStack->top();
		buildStack->pop();

		// Link rhs.
		IExpNode* expPtr = dynamic_cast<IExpNode*>(item);
		if (expPtr != nullptr) {m_cond = expPtr; continue;}

		// Otherwise, process other tokens.
		if (item->getBuildType() == SCAN_IF) {/* end of expr */ break;}
		else {/* Related, but unused */ delete item;}
	}

	// Use if operator for line number/location.
	m_lineNum = item->getLineNum();
	delete item;

	// Populate node's stmt list (in reverse- stack was LIFO).
	for (int i = stmtBuffer.size(); i > 0; i--) {
		m_stmtList.push_back(stmtBuffer.at(i - 1));
	}
}

// TODO
std::string IfNode::toString(void) {
	// Prepare string (with punctuation).
	string tknStr = "{";

	// Add node type using conversion function.
	tknStr += IASTNode::nodeToString(PARSE_IF_NODE);

	// Add exp info.
	if (m_cond->isUnit()) {tknStr += " using UNIT_EXPR";}
	else if (m_cond->isUnary()) {tknStr += " using UNARY_EXPR";}

	// Add stmt info.
	tknStr += ": " + to_string(m_stmtList.size()) + " stmts";

	// Finish punctuation and return.
	return tknStr + "}";
}

///////////////////////
// === WhileNode === //
///////////////////////

// TODO
WhileNode::WhileNode(std::stack<IBuildItem*>* buildStack) {
	// Temporary buffer to hold popped stmts.
	vector<IStmtNode*> stmtBuffer;

	// Process "body" tokens until un-related token is found.
	IBuildItem* item;
	while (buildStack->size()) { // Use stack size out of uniformity
		// Get next token.
		item = buildStack->top();
		buildStack->pop();

		// Pop "stmts" into buffer.
		IStmtNode* stmtPtr = dynamic_cast<IStmtNode*>(item);
		if (stmtPtr != nullptr) {stmtBuffer.push_back(stmtPtr); continue;}

		// Otherwise, process other tokens.
		if (item->getBuildType() == SCAN_LCURLY) {/* end of expr */ break;}
		else {/* Related, but unused */ delete item;}
	}
	delete item; // Throw out left curly

	// Process "conditional" tokens until un-related token is found.
	while (buildStack->size()) { // Use stack size out of uniformity
		// Get next token.
		item = buildStack->top();
		buildStack->pop();

		// Link rhs.
		IExpNode* expPtr = dynamic_cast<IExpNode*>(item);
		if (expPtr != nullptr) {m_cond = expPtr; continue;}

		// Otherwise, process other tokens.
		if (item->getBuildType() == SCAN_WHILE) {/* end of expr */ break;}
		else {/* Related, but unused */ delete item;}
	}

	// Use while operator for line number/location.
	m_lineNum = item->getLineNum();
	delete item;

	// Populate node's stmt list (in reverse- stack was LIFO).
	for (int i = stmtBuffer.size(); i > 0; i--) {
		m_stmtList.push_back(stmtBuffer.at(i - 1));
	}
}

// TODO
std::string WhileNode::toString(void) {
	// Prepare string (with punctuation).
	string tknStr = "{";

	// Add node type using conversion function.
	tknStr += IASTNode::nodeToString(PARSE_WHILE_NODE);

	// Add exp info.
	if (m_cond->isUnit()) {tknStr += " using UNIT_EXPR";}
	else if (m_cond->isUnary()) {tknStr += " using UNARY_EXPR";}

	// Add stmt info.
	tknStr += ": " + to_string(m_stmtList.size()) + " stmts";

	// Finish punctuation and return.
	return tknStr + "}";
}

//////////////////////
// === CallNode === //
//////////////////////

// TODO
CallNode::CallNode(std::stack<IBuildItem*>* buildStack) {
	// Temporary buffer to hold popped exps.
	vector<IExpNode*> expBuffer;

	// Process tokens until un-related token is found (or stack is empty).
	IBuildItem* item;
	int parenCnt = 0;
	while (buildStack->size()) { // Use stack size out of uniformity
		// Get next token.
		item = buildStack->top();
		buildStack->pop(); // "Inclusive" consumption of bracket.

		// Pop "exps" into buffer.
		IExpNode* expPtr = dynamic_cast<IExpNode*>(item);
		if (expPtr != nullptr && parenCnt != 0) {
			expBuffer.push_back(expPtr); continue;
		}

		// Update paren counter (don't parse func id as expression).
		uint8_t type = item->getBuildType();
		if (type == SCAN_LPAREN) {parenCnt--;}
		else if (type == SCAN_RPAREN) {parenCnt++;}

		// Otherwise, process other tokens.
		if (type == PARSE_ID_NODE) {/* End of body */ break;}
		else {/* Related, but unused */ delete item;}
	}

	// Process name of the call.
	m_id = dynamic_cast<IDNode*>(item);
	m_lineNum = item->getLineNum();

	// Populate node's arg list (in reverse- stack was LIFO).
	for (int i = expBuffer.size(); i > 0; i--) {
		m_argList.push_back(expBuffer.at(i - 1));
	}
}

// TODO
std::string CallNode::toString(void) {
	// Prepare string (with punctuation).
	string tknStr = "{";

	// Add node type using conversion function.
	tknStr += IASTNode::nodeToString(PARSE_CALL_NODE);

	// Add name info.
	tknStr += " named \"" + m_id->getValue() + "\"";

	// Add arg info.
	tknStr += " of " + to_string(m_argList.size()) + " args";

	// Finish punctuation and return.
	return tknStr + "}";
}
