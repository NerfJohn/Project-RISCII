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

#include <stdlib.h>
#include "MsgLog.h"

#include "NodeConcretes.h"

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

// TODO
void PrgmNode::checkSemantics(SymbolTable* symTable,
							  std::vector<Symbol*>* symList) {
	MsgLog::logINFO("Performing semantic checks...");

	// Pass on check to every decl/def.
	for (IDeclNode* decl : m_declList) {
		decl->checkSemantics(symTable, symList);

		// Set decl's symbol to "global".
		symTable->getLocal(decl->getValue())->m_isGlobal = true;
	}

	// Ensure an "int main()" exists.
	Symbol* mainSym = symTable->getLocal("main");
	if ((mainSym == nullptr) ||
		(mainSym->m_argList.size() != 0) ||
		(mainSym->m_type != TYPE_INT)) {
		MsgLog::logERR("\"int main()\" does not exist");
	}

	// Warn if any symbols haven't been used.
	mainSym->m_isUsed = true; // Corner case: main() is used w/o calling
	for (Symbol* sym: *symList) {
		if (!sym->m_isUsed) {
			MsgLog::logWARN("\"" + sym->m_id + "\" is unused",
							sym->m_declLineNum);
		}
	}

	MsgLog::logINFO("Finished semantic checks- " +
			to_string(symList->size()) + " symbols created");
}

// TODO
VarType_e PrgmNode::checkTyping(void) {
	MsgLog::logINFO("Performing type checks...");

	// Pass check to all children.
	for (IDeclNode* decl: m_declList) {
		decl->checkTyping(); // Returned typing unimportant at this level
	}

	MsgLog::logINFO("Finished type checks- all primary error checks passed");

	// Return value unimportant.
	return (VarType_e)(-1);
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

// TODO
void VarDeclNode::checkSemantics(SymbolTable* symTable,
								 std::vector<Symbol*>* symList) {
	// Declaration- add new name/symbol pair.
	Symbol* newSym = symTable->addSym(m_id);
	if (newSym == nullptr) {
		// Symbol already exists- log error.
		MsgLog::logERR("\"" + m_id + "\" multiply declared", m_lineNum);
	}
	newSym->m_isFunc = false;
	newSym->m_type = m_varType;
	newSym->m_declLineNum = m_lineNum;
	newSym->m_id = m_id;
	newSym->m_isInit |= m_isParam; // Params are always initialized
	symList->push_back(newSym);

	MsgLog::logINFO("New Symbol " + newSym->toName());
}

/////////////////////////
// === FuncDefNode === //
/////////////////////////

// TODO
FuncDefNode::FuncDefNode(std::stack<IBuildItem*>* buildStack) {
	// Temporary buffers to hold popped params/stmts.
	vector<VarDeclNode*> paramBuffer;
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
		VarDeclNode* declPtr = dynamic_cast<VarDeclNode*>(item);
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

// TODO
void FuncDefNode::checkSemantics(SymbolTable* symTable,
								 std::vector<Symbol*>* symList) {
	// Declaration- add new name/symbol pair.
	Symbol* newSym = symTable->addSym(m_id);
	if (newSym == nullptr) {
		// Symbol already exists- log error.
		MsgLog::logERR("\"" + m_id + "\" multiply declared", m_lineNum);
	}
	newSym->m_isFunc = true;
	newSym->m_type = m_varType;
	newSym->m_declLineNum = m_lineNum;
	newSym->m_id = m_id;
	for (VarDeclNode* type: m_paramList) {
		newSym->m_argList.push_back(type->getType());
	}
	symList->push_back(newSym);

	MsgLog::logINFO("New Symbol " + newSym->toName());

	// Process params/stmts in new, lower scope.
	symTable->pushScope();
	MsgLog::logINFO("Pushing scope " + to_string(symTable->getScopeNum()));
	for (VarDeclNode* decl : m_paramList) {
		decl->initParam();
		decl->checkSemantics(symTable, symList);
	}
	for (IStmtNode* stmt : m_stmtList) {
		stmt->checkSemantics(symTable, symList);
	}
	MsgLog::logINFO("Popping scope " + to_string(symTable->getScopeNum()));
	symTable->popScope();
}

// TODO
VarType_e FuncDefNode::checkTyping(void) {
	// Claim "current function" title.
	IASTNode::m_curFunc = this;

	// Pass check to statements (params don't need check).
	for (uint32_t i = 0; i < m_stmtList.size(); i++) {
		// Get node in question.
		IStmtNode* stmt = m_stmtList[i];

		// Pass down check.
		stmt->checkTyping();

		// Warn if unreachable lines exist in function's top scope.
		if (dynamic_cast<RetNode*>(stmt) != nullptr &&
			(i+1) < m_stmtList.size() &&
			!m_unreachTriggered) {
			MsgLog::logWARN("Lines past return are unreachable",
					stmt->getLineNum());
			m_unreachTriggered = true;
		}
	}

	// Warn if function has no obvious return paths.
	if (!m_hasRet) {
		MsgLog::logWARN("Function \"" + m_id + "\" has no obvious return",
				m_lineNum);
	}

	// Return typing is unimportant.
	return (VarType_e)(-1);
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

	// (Symbol will be set later).
	m_sym = nullptr;
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

// TODO
void IDNode::checkSemantics(SymbolTable* symTable,
							std::vector<Symbol*>* symList) {
	// Find symbol to link to.
	m_sym = symTable->getGlobal(m_id);
	if (m_sym == nullptr) {
		// ID is undeclared.
		MsgLog::logERR("\"" + m_id + "\" is undeclared", m_lineNum);
	}

	MsgLog::logINFO("Ref to " + m_sym->toName() +
			" from " + to_string(m_lineNum));

	// Fill in semantic information.
	m_sym->m_isInit |= m_isInit;
	m_sym->m_isUsed = true; // Counting writes- didn't prep my code for this...

	// Warn user if ID is being used before init.
	if (!m_sym->m_isInit) {
		MsgLog::logWARN("\"" + m_id + "\" is uninitialized", m_lineNum);
	}
}

// TODO
VarType_e IDNode::checkTyping(void) {
	// IDs represent variables- ensure this is true.
	if (m_sym->m_isFunc) {
		MsgLog::logERR("\"" + m_id + "\" is not a variable", m_lineNum);
	}

	// Return type being represented.
	return m_sym->m_type;
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
	m_intValue = 0;

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

// TODO
VarType_e LITNode::checkTyping(void) {
	// Process based on type of literal.
	bool tooLarge = false;
	if (m_strValue[0] == '\'') { // Char literal
		// Handle (most) chars.
		m_intValue = m_strValue[1];

		// Handle escaped chars.
		if (m_intValue == '\\') {
			switch (m_strValue[2]) {
				case '0': m_intValue = '\0'; break;
				case 't': m_intValue = '\t'; break;
				case 'n': m_intValue = '\n'; break;
				case '\\': m_intValue = '\\'; break;
				case '\'': m_intValue = '\''; break;
			}
		}
	}
	else if (m_strValue.find('x') != string::npos) { // Hex literal
		// Parse in each value-related char.
		m_intValue = 0;
		int multiplier = 1;
		for (int i = m_strValue.size(); i > 2; i--) {
			// Get the value of the character.
			int val = m_strValue[i-1];
			if (val <= '9') {val -= '0';}
			else if (val <= 'F') {val -= 'A' - 10;}
			else {val -= 'a' - 10;}

			// Add it to the sum value.
			m_intValue += (val * multiplier);

			// Adjust variables.
			if (m_intValue > 65535) {tooLarge = true;}
			multiplier *= 16;
		}
	}
	else { // Int literal
		// Parse in each value-related char.
		m_intValue = 0;
		int multiplier = 1;
		for (int i = m_strValue.size(); i > 0; i--) {
			// Get the value of the character.
			int val = m_strValue[i-1] - '0';

			// Add it to the sum value.
			val *= multiplier;
			m_intValue = (m_negateInt) ? m_intValue - val: m_intValue + val;

			// Adjust variables.
			if (m_intValue > 32767) {tooLarge = true;}
			if (m_intValue < -32768) {tooLarge = true;}
			multiplier *= 10;
		}
	}

	// Warn if literal couldn't fit/be properly represented.
	if (tooLarge) {
		MsgLog::logWARN("\"" + m_strValue + "\" does not fit within an INT-" +
				"defaulted to UINT for translation", m_lineNum);
	}

	// Return our general type.
	return TYPE_LITERAL;
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

// TODO
void AssignNode::checkSemantics(SymbolTable* symTable,
								std::vector<Symbol*>* symList) {
	// Mark lhs as initialized (before lhs call checks itself for init).
	m_lhs->initID();

	// Pass to children (following assign's right->left assoc.).
	m_rhs->checkSemantics(symTable, symList);
	m_lhs->checkSemantics(symTable, symList);
}

// TODO
VarType_e AssignNode::checkTyping(void) {
	// Pass check down (following assign's right->left assoc.).
	VarType_e rhsType = m_rhs->checkTyping();
	VarType_e lhsType = m_lhs->checkTyping();

	// Warn if assignment may be lossy.
	string rhsStr = VarType::toString(rhsType);
	string lhsStr = VarType::toString(lhsType);
	if (!IASTNode::canAssignTyping(lhsType, rhsType)) {
		MsgLog::logWARN("Assigning a " + rhsStr + " to a "
				+ lhsStr + " may be lossy", m_lineNum);
	}

	MsgLog::logINFO(rhsStr + " assigned to " + lhsStr +
			" at " + to_string(m_lineNum));

	// Return is unimportant.
	return (VarType_e)(-1);
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

// TODO
void RetNode::checkSemantics(SymbolTable* symTable,
							 std::vector<Symbol*>* symList) {
	// Pass to child.
	m_exp->checkSemantics(symTable, symList);
}

// TODO
VarType_e RetNode::checkTyping(void) {
	// Pass down.
	VarType_e expType =  m_exp->checkTyping();

	// Warn if return is lossy.
	VarType_e retType = IASTNode::m_curFunc->getType();
	string expStr = VarType::toString(expType);
	string retStr = VarType::toString(retType);
	if (!IASTNode::canAssignTyping(retType, expType)) {
		MsgLog::logWARN("Returning a " + expStr + " as a " +
				retStr + " may be lossy", m_lineNum);
	}

	MsgLog::logINFO(expStr + " returned from " + retStr +
					" function at " + to_string(m_lineNum));

	// Return present- set function's return flag.
	m_curFunc->setReturn(true);

	// Return original value.
	return expType;
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

// TODO
void IfNode::checkSemantics(SymbolTable* symTable,
							std::vector<Symbol*>* symList) {
	// First check the condition.
	m_cond->checkSemantics(symTable, symList);

	// Then check the block within new scope.
	symTable->pushScope();
	MsgLog::logINFO("Pushing scope " + to_string(symTable->getScopeNum()));
	for (IStmtNode* stmt: m_stmtList) {
		stmt->checkSemantics(symTable, symList);
	}
	MsgLog::logINFO("Popping scope " + to_string(symTable->getScopeNum()));
	symTable->popScope();
}

// TODO
VarType_e IfNode::checkTyping(void) {
	// Pass check to condition.
	m_cond->checkTyping();

	// Warn if conditional is obviously going to always/never trigger.
	LITNode* litPtr = dynamic_cast<LITNode*>(m_cond);
	if (litPtr != nullptr) {
		MsgLog::logWARN("Condition is constant \"" +
				m_cond->getValue() + "\"", m_lineNum);
	}

	// Pass check to statements (params don't need check).
	for (uint32_t i = 0; i < m_stmtList.size(); i++) {
		// Get node in question.
		IStmtNode* stmt = m_stmtList[i];

		// Pass down check.
		stmt->checkTyping();

		// Warn if unreachable lines exist in function's top scope.
		if (dynamic_cast<RetNode*>(stmt) != nullptr &&
			(i+1) < m_stmtList.size() &&
			!m_unreachTriggered) {
			MsgLog::logWARN("Lines past return are unreachable",
					stmt->getLineNum());
			m_unreachTriggered = true;
		}
	}

	// Override function's "return search" if block will not run.
	if ((litPtr == nullptr) || (litPtr->getIntValue() == 0)) {
		m_curFunc->setReturn(false);
	}

	// Return is unimportant.
	return (VarType_e)(-1);
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

// TODO
void WhileNode::checkSemantics(SymbolTable* symTable,
							   std::vector<Symbol*>* symList) {
	// First check the condition.
	m_cond->checkSemantics(symTable, symList);

	// Then check the block within new scope.
	symTable->pushScope();
	MsgLog::logINFO("Pushing scope " + to_string(symTable->getScopeNum()));
	for (IStmtNode* stmt: m_stmtList) {
		stmt->checkSemantics(symTable, symList);
	}
	MsgLog::logINFO("Popping scope " + to_string(symTable->getScopeNum()));
	symTable->popScope();
}

// TODO
VarType_e WhileNode::checkTyping(void) {
	// Pass check to condition.
	m_cond->checkTyping();

	// Warn if conditional is obviously going to always/never trigger.
	LITNode* litPtr = dynamic_cast<LITNode*>(m_cond);
	if (litPtr != nullptr) {
		MsgLog::logWARN("Condition is constant \"" +
				m_cond->getValue() + "\"", m_lineNum);
	}

	// Pass check to statements (params don't need check).
	for (uint32_t i = 0; i < m_stmtList.size(); i++) {
		// Get node in question.
		IStmtNode* stmt = m_stmtList[i];

		// Pass down check.
		stmt->checkTyping();

		// Warn if unreachable lines exist in function's top scope.
		if (dynamic_cast<RetNode*>(stmt) != nullptr &&
			(i+1) < m_stmtList.size() &&
			!m_unreachTriggered) {
			MsgLog::logWARN("Lines past return are unreachable",
					stmt->getLineNum());
			m_unreachTriggered = true;
		}
	}

	// Override function's "return search" if block will not run.
	if ((litPtr != nullptr) && (litPtr->getIntValue() == 0)) {
		m_curFunc->setReturn(false);
	}

	// Return is unimportant.
	return (VarType_e)(-1);
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

	// (Symbol will be linked later.)
	m_sym = nullptr;
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

// TODO
void CallNode::checkSemantics(SymbolTable* symTable,
							  std::vector<Symbol*>* symList) {
	// Use id's info to link to symbol from call.
	m_sym = symTable->getGlobal(m_id->getValue());
	if (m_sym == nullptr) {
		// ID is undeclared.
		MsgLog::logERR("\"" + m_id->getValue() + "\" is undeclared", m_lineNum);
	}

	MsgLog::logINFO("Ref to " + m_sym->toName() +
			" from " + to_string(m_lineNum));

	// Fill in semantic information.
	m_sym->m_isUsed = true; // Counting writes- didn't prep my code for this...

	// Pass check to arguments.
	for (IExpNode* arg: m_argList) {
		arg->checkSemantics(symTable, symList);
	}
}

// TODO
VarType_e CallNode::checkTyping(void) {
	// Calls represent functions- ensure ID is to a function.
	if (!m_sym->m_isFunc) {
		MsgLog::logERR("\"" + m_id->getValue() +
				"\" is not a function", m_lineNum);
	}

	// Ensure number of arguments is correct.
	if (m_argList.size() != m_sym->m_argList.size()) {
		string numArgs = to_string(m_argList.size());
		string numParams = to_string(m_sym->m_argList.size());
		string funcName = m_sym->m_id;
		MsgLog::logERR("\"" + funcName + "\" has " + numParams +
				" params- " + numArgs + " args given", m_lineNum);
	}

	// Pass check down.
	for (uint32_t i = 0; i < m_argList.size(); i++) {
		// Pass down check.
		VarType_e argType = m_argList[i]->checkTyping();

		// Warn if param/arg typing may result in loss.
		string paramStr = VarType::toString(m_sym->m_argList[i]);
		string argStr = VarType::toString(argType);
		if (!IASTNode::canAssignTyping(m_sym->m_argList[i], argType)) {
			MsgLog::logWARN("Using a " + argStr + " for a " +
					paramStr + " param may be lossy", m_lineNum);
		}

		MsgLog::logINFO(argStr + " passed to " + paramStr +
				" param at " + to_string(m_lineNum));
	}

	// Return expected return type.
	return m_sym->m_type;
}

///////////////////////
// === MinusNode === //
///////////////////////

// TODO
VarType_e MinusNode::checkTyping(void) {
	// Negate literal if acting as unary minus for int literal.
	LITNode* litPtr = dynamic_cast<LITNode*>(m_rhs);
	if (isUnary() && (litPtr != nullptr)) {litPtr->negateInt();}

	// Pass to children (as available).
	VarType_e lhsType= TYPE_LITERAL;
	VarType_e rhsType = TYPE_LITERAL;
	if (m_lhs != nullptr) {lhsType = m_lhs->checkTyping();}
	if (m_rhs != nullptr) {rhsType = m_rhs->checkTyping();}

	// Determine new typing to pass up.
	VarType_e newType = IASTNode::getNewTyping(lhsType, rhsType, m_lineNum);

	string lhsStr = (m_lhs != nullptr) ? VarType::toString(lhsType) + "," : "";
	string rhsStr = VarType::toString(rhsType);
	string newStr = VarType::toString(newType);
	string nodeStr = IASTNode::nodeToString((ParseActions)getBuildType());
	MsgLog::logINFO("(" + lhsStr + rhsStr + ") -> " + nodeStr +
			" = " + newStr + " at " + to_string(m_lineNum));

	// Pass accumulated type.
	return newType;
}
