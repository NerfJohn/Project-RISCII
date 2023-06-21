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
		Symbol* sym = symTable->getLocal(decl->getValue());

		// Set decl's symbol to "global".
		sym->m_isGlobal = true;

		// If a variable decl, assign it a global address (for future use).
		if (dynamic_cast<IDeclNode*>(decl) != nullptr) {
			sym->m_memValue = IASTNode::m_nextDataAddr;
			m_nextDataAddr += sizeof(uint16_t);
		}
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

// TODO
int PrgmNode::optimizeAST(std::unordered_map<Symbol*,int>* constList) {
	MsgLog::logINFO("Performing optimizations...");

	// Pass down optimization to children.
	vector<IDeclNode*> goodDecls;
	for (IDeclNode* decl: m_declList) {
		// Pass down.
		int action = decl->optimizeAST(constList);

		// Prune if requested.
		if (action == OPT_DEL_THIS) {
			string type =
					IASTNode::nodeToString((ParseActions)decl->getBuildType());
			MsgLog::logINFO(type + " named \""
					+ decl->getValue() + "\" deleted");
			delete decl;
		}
		else {goodDecls.push_back(decl);}
	}

	MsgLog::logINFO("Optimizations complete- " +
			to_string(m_numDeletes) + " nodes pruned");

	// Return value unimportant.
	m_declList = goodDecls;
	return OPT_KEEP;
}

// TODO
genValue_t PrgmNode::translate(AsmMaker* asmGen) {
	MsgLog::logINFO("Translating to assembly...");

	// Generate "BIOS" that initializes stack, calls, main, and returns/halts.
	asmGen->addComment("\"BIOS\"- init stack, run main, and return/halt");
	asmGen->genInstr({.type=INSTR_LBI, .r1=REG_SP, .imm=0,
		.cmt="Begin stack/frame pointers at 0x10000"});
	asmGen->genInstr({.type=INSTR_LBI, .r1=REG_FP, .imm=0});
	asmGen->genCall("main");
	asmGen->genInstr({.type=INSTR_STR, .r1=REG_AC, .r2=REG_FP,
		.imm=0, .cmt="Store result  to 0x0 (in cache) for testing purposes"});
	asmGen->genInstr({.type=INSTR_HLT});
	asmGen->addSpacer();

	// Generate children's assembly code.
	for (IDeclNode* decl: m_declList) {decl->translate(asmGen);}

	MsgLog::logINFO("AST translated- " + to_string(asmGen->numInstrs()) +
			" instructions (and " + to_string(asmGen->numMacros()) +
			" macros) generated");

	// Ensure projected image size can fit in processor's 64-KB memory.
	uint32_t imageSize = (asmGen->numInstrs() + (3 * asmGen->numMacros())) << 1;
	if (imageSize > 0xffff) {
		MsgLog::logERR("Projected image size (" + to_string(imageSize) +
				" bytes) is too large for the target processor (65536 bytes)");
	}

	// Return is irrelevant.
	return {};
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
	newSym->m_constVal = OPT_VAL_UNKNOWN;
	symList->push_back(newSym);
	m_sym = newSym;

	// Assign a local/stack address if within a function.
	if (m_curFunc != nullptr) {
		FuncDefNode* curFunc = dynamic_cast<FuncDefNode*>(m_curFunc);
		newSym->m_memValue = curFunc->m_nextLocalAddr;
		curFunc->m_nextLocalAddr -= sizeof(uint16_t);
	}

	MsgLog::logINFO("New Symbol " + newSym->toName());
}

// TODO
int VarDeclNode::optimizeAST(std::unordered_map<Symbol*,int>* constList) {
	// Request deletion if unused.
	if (!m_sym->m_isUsed) {
		return OPT_DEL_THIS;
	}

	// Otherwise- cannot optimize a decl.
	return OPT_KEEP;
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

	// (Appease compiler overlords.)
	s_accumIsInter = false;
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
	// Claim "current function" title.
	IASTNode::m_curFunc = this;

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
	m_sym = newSym;

	MsgLog::logINFO("New Symbol " + newSym->toName());

	// Process params/stmts in new, lower scope.
	int memOffset = 4;
	symTable->pushScope();
	MsgLog::logINFO("Pushing scope " + to_string(symTable->getScopeNum()));
	for (VarDeclNode* decl : m_paramList) {
		// Process/analyze params.
		decl->initParam();
		decl->checkSemantics(symTable, symList);

		// Assign an memory offset to the param (for future use).
		symTable->getLocal(decl->getValue())->m_memValue = memOffset;
		memOffset += sizeof(uint16_t);
	}
	for (IStmtNode* stmt : m_stmtList) {
		stmt->checkSemantics(symTable, symList);
	}
	MsgLog::logINFO("Popping scope " + to_string(symTable->getScopeNum()));
	symTable->popScope();

	// Drop "current function" title.
	IASTNode::m_curFunc = nullptr;
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

// TODO
int FuncDefNode::optimizeAST(std::unordered_map<Symbol*,int>* constList) {
	// Request deletion if unused.
	if (!m_sym->m_isUsed) {
		return OPT_DEL_THIS;
	}

	// Otherwise, pass down to stmts (with new const list if applicable).
	bool burnEmAll = false;
	vector<IStmtNode*> goodStmts;
	if (constList != nullptr) {constList->clear();}
	for (IStmtNode* stmt: m_stmtList) {
		// Pass down (if applicable).
		int stmtValue = OPT_KEEP;
		if (!burnEmAll) {stmtValue = stmt->optimizeAST(constList);}

		// Handle deletions.
		if (stmtValue == OPT_DEL_THIS ||
			stmtValue == OPT_BURN_INCLUDE ||
			burnEmAll) {
			string type =
					IASTNode::nodeToString((ParseActions)stmt->getBuildType());
			MsgLog::logINFO(type + " at " + to_string(stmt->getLineNum())
					+ " deleted");
			delete stmt;
		}
		else {
			goodStmts.push_back(stmt);
		}

		burnEmAll |= (stmtValue == OPT_BURN_EXCLUDE ||
				 	  stmtValue == OPT_BURN_INCLUDE);
	}

	// Cannot optimize a function def.
	m_stmtList = goodStmts;
	return OPT_KEEP;
}

// TODO
genValue_t FuncDefNode::translate(AsmMaker* asmGen) {
	// Claim "current function" title.
	IASTNode::m_curFunc = this;

	// Determine frame size for this function (RA/FP + locals + saves).
	int frameSize = 4 + -(m_nextLocalAddr - -2) + 8;

	// Clear load registers- memory handler shouldn't rely on those values.
	MemHandler::clearLoads();

	// Generate function's "preamble"- starting with framing...
	asmGen->addComment("=== Function " + m_id + " ===");
	asmGen->addLabel(AsmMaker::FUNC_PREFIX + m_id);
	asmGen->addSpacer();
	asmGen->addComment("Push address and return link");
	asmGen->genInstr({.type=INSTR_STR, .r1=REG_RA, .r2=REG_SP, .imm=-2});
	asmGen->genInstr({.type=INSTR_STR, .r1=REG_FP, .r2=REG_SP, .imm=-4});
	asmGen->addSpacer();
	asmGen->addComment("Setup new frame");
	asmGen->genInstr({.type=INSTR_ADD, .r1=REG_FP, .r2=REG_SP, .imm=-4});
	MemHandler::loadLitToReg(asmGen, REG_RA, frameSize);
	asmGen->genInstr({.type=INSTR_SUB, .r1=REG_SP, .r2=REG_RA, .r3=REG_SP});
	asmGen->addSpacer();

	// ... followed by callee saves.
	asmGen->addComment("Callee save load registers");
	asmGen->genInstr({.type=INSTR_STR, .r1=REG_0, .r2=REG_SP, .imm=6});
	asmGen->genInstr({.type=INSTR_STR, .r1=REG_1, .r2=REG_SP, .imm=4});
	asmGen->genInstr({.type=INSTR_STR, .r1=REG_2, .r2=REG_SP, .imm=2});
	asmGen->genInstr({.type=INSTR_STR, .r1=REG_3, .r2=REG_SP, .imm=0});
	asmGen->addSpacer();

	// Generate children's code.
	for (IStmtNode* stmt: m_stmtList) {
		// New stmt- stack should be clear of intermediates.
		s_accumIsInter = false;

		// Generate child code.
		stmt->translate(asmGen);
	}

	// Generate function's "return"- starting with callee restores...
	asmGen->addLabel(AsmMaker::RET_PREFIX + m_id);
	asmGen->addComment("Callee restore load registers");
	asmGen->genInstr({.type=INSTR_LDR, .r1=REG_0, .r2=REG_SP, .imm=6});
	asmGen->genInstr({.type=INSTR_LDR, .r1=REG_1, .r2=REG_SP, .imm=4});
	asmGen->genInstr({.type=INSTR_LDR, .r1=REG_2, .r2=REG_SP, .imm=2});
	asmGen->genInstr({.type=INSTR_LDR, .r1=REG_3, .r2=REG_SP, .imm=0});
	asmGen->addSpacer();

	// .. followed by framing/returning.
	asmGen->addComment("Strike new frame");
	asmGen->genInstr({.type=INSTR_ADD, .r1=REG_SP, .r2=REG_FP, .imm=4});
	asmGen->genInstr({.type=INSTR_LDR, .r1=REG_FP, .r2=REG_FP, .imm=0});
	asmGen->addSpacer();
	asmGen->addComment("Return to caller");
	asmGen->genInstr({.type=INSTR_LDR, .r1=REG_RA, .r2=REG_SP, .imm=-2});
	asmGen->genInstr({.type=INSTR_JPR, .r1=REG_RA, .imm=0});
	asmGen->addSpacer();

	// Drop "current function" title.
	IASTNode::m_curFunc = nullptr;

	MsgLog::logINFO(m_id + "() at " + to_string(m_lineNum) + " translated");

	// Irrelevant return.
	return {};
}

// TODO
bool FuncDefNode::isLastStmt(IStmtNode* stmt) {
	return stmt == m_stmtList[m_stmtList.size() - 1];
}

// TODO
FuncDefNode::~FuncDefNode(void) {
	// Prune connected nodes.
	for (IStmtNode* stmt: m_stmtList) {delete stmt;}
	for (VarDeclNode* param: m_paramList) {delete param;}
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

// TODO
int IDNode::optimizeAST(std::unordered_map<Symbol*,int>* constList) {
	// Save constant value if available (translator decides ID vs. LIT).
	if (constList != nullptr &&
		constList->find(m_sym) != constList->end()) {
		m_constVal = (*constList)[m_sym];

		MsgLog::logINFO("\"" + m_id + "\" at " + to_string(m_lineNum)
				+ " marked as const " + to_string(m_constVal));

		return m_constVal;
	}

	// Always save IDs.
	return OPT_KEEP;
}

// TODO
genValue_t IDNode::translate(AsmMaker* asmGen) {
	// Prep ID to be loaded by parent.
	if (m_constVal != OPT_VAL_UNKNOWN) {m_sym->m_constVal = m_constVal;}

	// Return ID's symbol info for loading.
	genValue_t retVal = {.type=KEY_SYMBOL, .data=(int)(m_sym)};
	return retVal;
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
	m_constVal = 0;

	// Consume/free token.
	buildStack->pop();
	delete item;
}

// TODO
LITNode::LITNode(int value) {
	// Set constant value- only used part at this point.
	m_constVal = value;
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
		m_constVal = m_strValue[1];

		// Handle escaped chars.
		if (m_constVal == '\\') {
			switch (m_strValue[2]) {
				case '0': m_constVal = '\0'; break;
				case 't': m_constVal = '\t'; break;
				case 'n': m_constVal = '\n'; break;
				case '\\': m_constVal = '\\'; break;
				case '\'': m_constVal = '\''; break;
			}
		}
	}
	else if (m_strValue.find('x') != string::npos) { // Hex literal
		// Parse in each value-related char.
		m_constVal = 0;
		int multiplier = 1;
		for (int i = m_strValue.size(); i > 2; i--) {
			// Get the value of the character.
			int val = m_strValue[i-1];
			if (val <= '9') {val -= '0';}
			else if (val <= 'F') {val -= 'A' - 10;}
			else {val -= 'a' - 10;}

			// Add it to the sum value.
			m_constVal += (val * multiplier);

			// Adjust variables.
			if (m_constVal > 65535) {tooLarge = true;}
			multiplier *= 16;
		}
	}
	else { // Int literal
		// Parse in each value-related char.
		m_constVal = 0;
		int multiplier = 1;
		for (int i = m_strValue.size(); i > 0; i--) {
			// Get the value of the character.
			int val = m_strValue[i-1] - '0';

			// Add it to the sum value.
			val *= multiplier;
			m_constVal = (m_negateInt) ? m_constVal - val: m_constVal + val;

			// Adjust variables.
			if (m_constVal > 32767) {tooLarge = true;}
			if (m_constVal < -32768) {tooLarge = true;}
			multiplier *= 10;
		}

		// (Undo negation- just needed it for sizing check).
		if (m_negateInt) {m_constVal = -m_constVal;}
	}

	// Warn if literal couldn't fit/be properly represented.
	if (tooLarge) {
		MsgLog::logWARN("\"" + m_strValue + "\" does not fit within an INT-" +
				" defaulted to UINT for translation", m_lineNum);
		return TYPE_UINT;
	}

	// Return our general type.
	return TYPE_LITERAL;
}

// TODO
int LITNode::optimizeAST(std::unordered_map<Symbol*,int>* constList) {
	// Parent prunes pairs of lits.
	return m_constVal;
}

// TODO
genValue_t LITNode::translate(AsmMaker* asmGen) {
	// Return packaged value for loading.
	genValue_t retGen = {.type=KEY_LITERAL, .data=m_constVal};
	return retGen;
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

// TODO
int AssignNode::optimizeAST(std::unordered_map<Symbol*,int>* constList) {
	// Pass down to expression (IDNode lives/dies with the statement).
	int rhsValue = m_rhs->optimizeAST(constList);

	// Add/modify the constant list if a "const propagation" to local.
	if (constList != nullptr &&
		rhsValue <= OPT_CONST_MAX &&
		!m_lhs->getSym()->m_isGlobal) {
		MsgLog::logINFO("Assigning \"" + m_lhs->getValue() + "\" at "
				+ to_string(m_lineNum) + " as const "
				+ to_string(rhsValue));
		(*constList)[m_lhs->getSym()] = rhsValue;
	}

	// Generally keep- parent decides fate.
	return OPT_KEEP;
}

// TODO
genValue_t AssignNode::translate(AsmMaker* asmGen) {
	// Generate expression's code.
	asmGen->addComment("Assignment statement");
	genValue_t expVal = m_rhs->translate(asmGen);

	// Load/determine register holding result.
	RegName_e resReg = MemHandler::load(asmGen, expVal);

	// Truncate result if lhs is shorter than word size.
	VarType_e resType = m_lhs->getSym()->m_type;
	if ((resType == TYPE_CHAR) ||
		(resType == TYPE_UCHAR)) {
		InstrFlag_e resFlg = (resType == TYPE_CHAR) ? FLAG_ARITH : FLAG_NONE;
		asmGen->genInstr({.type=INSTR_SHL, .r1=resReg,
			.r2=resReg, .imm=8, .cmt="Assigning 8-bit value"});
		asmGen->genInstr({.type=INSTR_SHR, .flg=resFlg, .r1=resReg,
			.r2=resReg, .imm=8});
	}

	// Store value to its stop in RAM.
	genValue_t lhsVal = m_lhs->translate(asmGen);
	MemHandler::store(asmGen, resReg, lhsVal);
	asmGen->addSpacer();

	// Update register file map for potential re-use.
	MemHandler::updateReg(resReg, lhsVal);

	// Irrelevant return.
	return {};
}

// TODO
AssignNode::~AssignNode(void) {
	// Prune children.
	delete m_lhs;
	delete m_rhs;
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

// TODO
int RetNode::optimizeAST(std::unordered_map<Symbol*,int>* constList) {
	// Pass down to expression.
	m_exp->optimizeAST(constList);

	// Anything past return (in scope) isn't run.
	return OPT_BURN_EXCLUDE;
}

// TODO
genValue_t RetNode::translate(AsmMaker* asmGen) {
	// Generate expression code.
	asmGen->addComment("Return statement");
	genValue_t expVal = m_exp->translate(asmGen);

	// Ensure result in accumulator for returning.
	RegName_e resReg = MemHandler::load(asmGen, expVal);
	if (resReg != REG_AC) {
		string cmt = "Moving to accumulator for return";
		asmGen->genInstr({.type=INSTR_ADD, .r1=REG_AC,
			.r2=resReg, .imm=0, .cmt=cmt});
	}

	// Truncate result if return type is shorter than word size.
	VarType_e resType = m_curFunc->getType();
	if ((resType == TYPE_CHAR) ||
		(resType == TYPE_UCHAR)) {
		InstrFlag_e resFlg = (resType == TYPE_CHAR) ? FLAG_ARITH : FLAG_NONE;
		asmGen->genInstr({.type=INSTR_SHL, .r1=REG_AC,
			.r2=REG_AC, .imm=8, .cmt="Assigning 8-bit value"});
		asmGen->genInstr({.type=INSTR_SHR, .flg=resFlg, .r1=REG_AC,
			.r2=REG_AC, .imm=8});
	}

	// Jump to function return (if not already at the end).
	if (!dynamic_cast<FuncDefNode*>(m_curFunc)->isLastStmt(this)) {
		asmGen->genToRet(m_curFunc->getValue());
	}
	asmGen->addSpacer();

	// Irrelevant return.
	return {};
}

// TODO
RetNode::~RetNode(void) {
	// Delete expression.
	delete m_exp;
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
	if ((litPtr == nullptr) || (litPtr->getConstVal() == 0)) {
		m_curFunc->setReturn(false);
	}

	// Return is unimportant.
	return (VarType_e)(-1);
}

// TODO
int IfNode::optimizeAST(std::unordered_map<Symbol*,int>* constList) {
	// Resolve condition- may be constant.
	int condVal = m_cond->optimizeAST(constList);

	// Delete if condition prevents block from running.
	if (condVal == 0) {return OPT_DEL_THIS;}

	// Save constants for "jump over block" case.
	unordered_map<Symbol*, int> saveList;
	if (constList != nullptr) {saveList = *constList;}

	// Otherwise, pass down to stmts (with new const list if applicable).
	bool burnEmAll = false;
	vector<IStmtNode*> goodStmts;
	for (IStmtNode* stmt: m_stmtList) {
		// Pass down (if applicable).
		int stmtValue = OPT_KEEP;
		if (!burnEmAll) {stmtValue = stmt->optimizeAST(constList);}

		// Handle deletions.
		if (stmtValue == OPT_DEL_THIS ||
			stmtValue == OPT_BURN_INCLUDE ||
			burnEmAll) {
			string type =
					IASTNode::nodeToString((ParseActions)stmt->getBuildType());
			MsgLog::logINFO(type + " at " + to_string(stmt->getLineNum())
					+ " deleted");
			delete stmt;
		}
		else {
			goodStmts.push_back(stmt);
		}

		burnEmAll |= (stmtValue == OPT_BURN_EXCLUDE ||
				 	  stmtValue == OPT_BURN_INCLUDE);
	}

	// Restore list if block won't always run.
	if (constList != nullptr &&
		(condVal > OPT_CONST_MAX ||
		condVal == 0)) {*constList = saveList;}

	// Parent generally decides statements fate.
	m_stmtList = goodStmts;
	return OPT_KEEP;
}

// TODO
genValue_t IfNode::translate(AsmMaker* asmGen) {
	// Generate/load condition.
	asmGen->addComment("If statement");
	genValue_t condVal = m_cond->translate(asmGen);
	RegName_e condReg = MemHandler::load(asmGen, condVal);

	// Generate conditional jump to label.
	string ifLabel = "if_" + asmGen->getNewLabel();
	string cmt = "(skip 3-instruction macro)";
	asmGen->genInstr({.type=INSTR_ADD, .r1=condReg, .r2=condReg,
		.imm=0, .cmt="Prep condition check"});
	asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_NP, .imm=6, .cmt=cmt});
	asmGen->genToLabel(ifLabel);
	asmGen->addSpacer();

	// Generate children's code.
	for (IStmtNode* stmt: m_stmtList) {
		// New stmt- stack should be clear of intermediates.
		s_accumIsInter = false;

		// Generate child code.
		stmt->translate(asmGen);
	}

	// Generate label for jumping over if block.
	asmGen->addLabel(ifLabel);
	asmGen->addSpacer();

	// Can't be sure if run- clear register file records.
	MemHandler::clearLoads();
	MemHandler::clearAccum();

	// Irrelevant return.
	return {};
}

// TODO
IfNode::~IfNode(void) {
	// Delete stored stmts/condition.
	delete m_cond;
	for (IStmtNode* stmt: m_stmtList) {delete stmt;}
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
	if ((litPtr != nullptr) && (litPtr->getConstVal() == 0)) {
		m_curFunc->setReturn(false);
	}

	// Return is unimportant.
	return (VarType_e)(-1);
}

// TODO
int WhileNode::optimizeAST(std::unordered_map<Symbol*,int>* constList) {
	// Resolve condition- may be constant.
	if (constList != nullptr) {constList->clear();}
	int condVal = m_cond->optimizeAST(constList);

	// Delete if condition prevents block from running.
	if (condVal == 0) {return OPT_DEL_THIS;}

	// Otherwise, pass down to stmts (with new const list if applicable).
	bool burnEmAll = false;
	vector<IStmtNode*> goodStmts;
	for (IStmtNode* stmt: m_stmtList) {
		// Pass down (if applicable).
		int stmtValue = OPT_KEEP;
		if (!burnEmAll) {stmtValue = stmt->optimizeAST(constList);}

		// Handle deletions.
		if (stmtValue == OPT_DEL_THIS ||
			stmtValue == OPT_BURN_INCLUDE ||
			burnEmAll) {
			string type =
					IASTNode::nodeToString((ParseActions)stmt->getBuildType());
			MsgLog::logINFO(type + " at " + to_string(stmt->getLineNum())
					+ " deleted");
			delete stmt;
		}
		else {
			goodStmts.push_back(stmt);
		}

		burnEmAll |= (stmtValue == OPT_BURN_EXCLUDE ||
				 	  stmtValue == OPT_BURN_INCLUDE);
	}

	// Prune following statements if loop never ends.
	if (condVal <= OPT_CONST_MAX && condVal != 0) {return OPT_BURN_EXCLUDE;}

	// Parent generally decides statements fate.
	m_stmtList = goodStmts;
	return OPT_KEEP;
}

// TODO
genValue_t WhileNode::translate(AsmMaker* asmGen) {
	// Generate label allowing for looping.
	asmGen->addComment("While statement");
	string loopLabel = "loop_" + asmGen->getNewLabel();
	asmGen->addLabel(loopLabel);

	// Can't be sure if looped- clear register file records.
	MemHandler::clearLoads();
	MemHandler::clearAccum();

	// Generate/load condition.
	genValue_t condVal = m_cond->translate(asmGen);
	RegName_e condReg = MemHandler::load(asmGen, condVal);

	// Generate conditional jump to label.
	string whileLabel = "while_" + asmGen->getNewLabel();
	string cmt = "(skip 3-instruction macro)";
	asmGen->genInstr({.type=INSTR_ADD, .r1=condReg, .r2=condReg,
		.imm=0, .cmt="Prep condition check"});
	asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_NP, .imm=6, .cmt=cmt});
	asmGen->genToLabel(whileLabel);
	asmGen->addSpacer();

	// Generate children's code.
	for (IStmtNode* stmt: m_stmtList) {
		// New stmt- stack should be clear of intermediates.
		s_accumIsInter = false;

		// Generate child code.
		stmt->translate(asmGen);
	}

	// Generate code to perform loop.
	asmGen->genToLabel(loopLabel);
	asmGen->addSpacer();

	// Generate label for jumping over while block.
	asmGen->addLabel(whileLabel);
	asmGen->addSpacer();

	// Can't be sure if run- clear register file records.
	MemHandler::clearLoads();
	MemHandler::clearAccum();

	// Irrelevant return.
	return {};
}

// TODO
WhileNode::~WhileNode(void) {
	// Delete stored stmts/condition.
	delete m_cond;
	for (IStmtNode* stmt: m_stmtList) {delete stmt;}
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

// TODO
int CallNode::optimizeAST(std::unordered_map<Symbol*,int>* constList) {
	// Pass to arguments.
	for (IExpNode* arg: m_argList) {
		arg->optimizeAST(constList);
	}

	// Generally up to scope to delete.
	return OPT_KEEP;
}

// TODO
genValue_t CallNode::translate(AsmMaker* asmGen) {
	// Outline call for ease of reading.
	asmGen->addComment("Function call to " + m_id->getValue());

	// Push any intermediates to stack for safety.
	if (s_accumIsInter) {
		genValue dummyInter = {.type=KEY_INTERMEDIATE};
		MemHandler::store(asmGen, REG_AC, dummyInter);
	}

	// Generate/push each argument in reverse (LIFO call setup).
	int argOffset = -2;
	for (int i = m_argList.size(); i > 0; i--) {
		// Generate/load expression.
		genValue_t argVal = m_argList[i-1]->translate(asmGen);
		RegName_e argReg = MemHandler::load(asmGen, argVal);

		// Truncate result if lhs is shorter than word size.
		VarType_e resType = m_sym->m_argList[i-1];
		if ((resType == TYPE_CHAR) ||
			(resType == TYPE_UCHAR)) {
			InstrFlag_e resFlg = (resType == TYPE_CHAR) ?
					FLAG_ARITH : FLAG_NONE;
			asmGen->genInstr({.type=INSTR_SHL, .r1=argReg,
				.r2=argReg, .imm=8, .cmt="Assigning 8-bit value"});
			asmGen->genInstr({.type=INSTR_SHR, .flg=resFlg, .r1=argReg,
				.r2=argReg, .imm=8});
		}

		// Push value on top of stack (Hack: use sym to re-use store logic).
		MemHandler::pushArg(asmGen, argReg, argOffset);
		asmGen->addSpacer();

		// Use next available slot for next arg.
		argOffset -= sizeof(uint16_t);
	}

	// Generate stack adjustments and call.
	RegName_e sizeReg = MemHandler::load(asmGen, {.type=KEY_LITERAL,
			.data=(int)(m_argList.size() * sizeof(uint16_t))});
	asmGen->genInstr({.type=INSTR_SUB, .r1=REG_SP, .r2=sizeReg, .r3=REG_SP});
	asmGen->genCall(m_id->getValue());
	asmGen->genInstr({.type=INSTR_ADD, .r1=REG_SP, .r2=sizeReg, .r3=REG_SP});
	asmGen->addSpacer();

	// Update register file mapping (use node ptr as unique ID).
	genValue_t retVal = {.type=KEY_INTERMEDIATE, .data=(int)(this)};
	MemHandler::updateReg(REG_AC, retVal);
	s_accumIsInter = true;

	// Return irrelevant.
	return retVal;
}

// TODO
CallNode::~CallNode(void) {
	// Delete children (doesn't own symbol- do not delete).
	delete m_id;
	for (IExpNode* arg: m_argList) {delete arg;}
}

//////////////////////
// === Op Nodes === //
//////////////////////

// TODO
void AndNode::computeConst() {
	m_constVal = m_lhs->getConstVal() & m_rhs->getConstVal();
	m_constVal &= 0xffff;
}

// TODO
void AndNode::genExp(AsmMaker* asmGen, RegName_e lhs, RegName_e rhs) {
	asmGen->genInstr({.type=INSTR_AND, .r1=REG_AC, .r2=lhs, .r3=rhs});
}

// TODO
void OrNode::computeConst() {
	m_constVal = m_lhs->getConstVal() | m_rhs->getConstVal();
	m_constVal &= 0xffff;
}

// TODO
void OrNode::genExp(AsmMaker* asmGen, RegName_e lhs, RegName_e rhs) {
	asmGen->genInstr({.type=INSTR_ORR, .r1=REG_AC, .r2=lhs, .r3=rhs});
}

// TODO
void XorNode::computeConst() {
	m_constVal = m_lhs->getConstVal() ^ m_rhs->getConstVal();
	m_constVal &= 0xffff;
}

// TODO
void XorNode::genExp(AsmMaker* asmGen, RegName_e lhs, RegName_e rhs) {
	asmGen->genInstr({.type=INSTR_XOR, .r1=REG_AC, .r2=lhs, .r3=rhs});
}

// TODO
void LNotNode::computeConst() {
	m_constVal = ~m_rhs->getConstVal();
	m_constVal &= 0xffff;
}

// TODO
void LNotNode::genExp(AsmMaker* asmGen, RegName_e lhs, RegName_e rhs) {
	asmGen->genInstr({.type=INSTR_XOR, .r1=REG_AC, .r2=rhs, .imm=-1});
}

// TODO
VarType_e RShiftNode::checkTyping(void) {
	// Pass to children (as available).
	VarType_e lhsType= TYPE_LITERAL;
	VarType_e rhsType = TYPE_LITERAL;
	if (m_lhs != nullptr) {lhsType = m_lhs->checkTyping();}
	if (m_rhs != nullptr) {rhsType = m_rhs->checkTyping();}

	// Type is based on left hand side ONLY.
	m_varType = lhsType;

	string lhsStr = (m_lhs != nullptr) ? VarType::toString(lhsType) + "," : "";
	string rhsStr = VarType::toString(rhsType);
	string newStr = VarType::toString(m_varType);
	string nodeStr = IASTNode::nodeToString((ParseActions)getBuildType());
	MsgLog::logINFO("(" + lhsStr + rhsStr + ") -> " + nodeStr +
			" = " + newStr + " at " + to_string(m_lineNum));

	// Pass accumulated type.
	return m_varType;
}

// TODO
void RShiftNode::computeConst() {
	// Lhs determines Determine logical vs. arithmetic.
	if (m_varType == TYPE_UINT ||
		m_varType == TYPE_UCHAR) { // Unsigned comparison
		m_constVal = (uint16_t)(m_lhs->getConstVal()) >>
				(uint16_t)(m_rhs->getConstVal());
	}
	else { // Signed- arithmetic
		m_constVal = (int16_t)(m_lhs->getConstVal()) >>
				(int16_t)(m_rhs->getConstVal());
	}
	m_constVal &= 0xffff;
}

// TODO
void RShiftNode::genExp(AsmMaker* asmGen, RegName_e lhs, RegName_e rhs) {
	// Lhs determines Determine logical vs. arithmetic.
	if (m_varType == TYPE_UINT ||
		m_varType == TYPE_UCHAR) { // Unsigned- logical
		asmGen->genInstr({.type=INSTR_AND, .r1=REG_RA,
			.r2=rhs, .imm=0xf});
		asmGen->genInstr({.type=INSTR_SHR, .flg=FLAG_NONE,
			.r1=REG_AC, .r2=lhs, .r3=REG_RA});
	}
	else { // Signed- arithmetic
		asmGen->genInstr({.type=INSTR_ORR, .r1=REG_RA,
			.r2=rhs, .imm=-16});
		asmGen->genInstr({.type=INSTR_SHR, .flg=FLAG_ARITH,
			.r1=REG_AC, .r2=lhs, .r3=REG_RA});
	}
}

// TODO
void LShiftNode::computeConst() {
	m_constVal = m_lhs->getConstVal() << m_rhs->getConstVal();
	m_constVal &= 0xffff;
}

// TODO
void LShiftNode::genExp(AsmMaker* asmGen, RegName_e lhs, RegName_e rhs) {
	asmGen->genInstr({.type=INSTR_SHL, .r1=REG_AC, .r2=lhs, .r3=rhs});
}

// TODO
void PlusNode::computeConst() {
	m_constVal = m_lhs->getConstVal() + m_rhs->getConstVal();
	m_constVal &= 0xffff;
}

// TODO
void PlusNode::genExp(AsmMaker* asmGen, RegName_e lhs, RegName_e rhs) {
	asmGen->genInstr({.type=INSTR_ADD, .r1=REG_AC, .r2=lhs, .r3=rhs});
}

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

// TODO
void MinusNode::computeConst() {
	int lhsVal = (m_lhs == nullptr) ? 0 : m_lhs->getConstVal();
	m_constVal = lhsVal - m_rhs->getConstVal();
	m_constVal &= 0xffff;
}

// TODO
void MinusNode::genExp(AsmMaker* asmGen, RegName_e lhs, RegName_e rhs) {
	if (lhs == BAD_REG) {
		asmGen->genInstr({.type=INSTR_SUB, .r1=REG_AC, .r2=rhs, .imm=0});
	}
	else {
		asmGen->genInstr({.type=INSTR_SUB, .r1=REG_AC, .r2=rhs, .r3=lhs});
	}
}

// TODO
void GrtNode::computeConst() {
	// Signed vs. Unsigned comparison.
	if (m_varType == TYPE_UINT ||
		m_varType == TYPE_UCHAR) { // Unsigned comparison
		m_constVal = (uint16_t)(m_lhs->getConstVal()) >
			(uint16_t)(m_rhs->getConstVal());
	}
	else { // Signed comparison
		m_constVal = (int16_t)(m_lhs->getConstVal()) >
			(int16_t)(m_rhs->getConstVal());
	}
	m_constVal &= 0xffff;
}

// TODO
void GrtNode::genExp(AsmMaker* asmGen, RegName_e lhs, RegName_e rhs) {
	// Move current result as needed to generate new one.
	string cmt = "Move operand for result processing";
	if (lhs == REG_AC) {
		asmGen->genInstr({.type=INSTR_ADD, .r1=REG_RA,
			.r2=lhs, .imm=0, .cmt=cmt});
		lhs = REG_RA;
	}
	else if (rhs == REG_AC) {
		asmGen->genInstr({.type=INSTR_ADD, .r1=REG_RA,
			.r2=rhs, .imm=0, .cmt=cmt});
		rhs = REG_RA;
	}

	// Signed vs. Unsigned comparison.
	if (m_varType == TYPE_UINT ||
		m_varType == TYPE_UCHAR) { // Unsigned comparison
		// Generate unsigned check.
		asmGen->genInstr({.type=INSTR_XOR, .r1=REG_AC, .r2=lhs, .r3=rhs});
		asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_N, .imm=10});
		asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=0});
		asmGen->genInstr({.type=INSTR_SUB, .r1=REG_RA, .r2=lhs, .r3=rhs});
		asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_ZP, .imm=8});
		asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=1});
		asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_NZP, .imm=4});
		asmGen->genInstr({.type=INSTR_AND, .r1=REG_AC, .r2=REG_AC, .r3=lhs});
		asmGen->genInstr({.type=INSTR_SHR, .r1=REG_AC, .r2=REG_AC, .imm=15});
	}
	else { // Signed comparison
		// Generate signed check.
		asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=1});
		asmGen->genInstr({.type=INSTR_SUB, .r1=REG_RA, .r2=lhs, .r3=rhs});
		asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_N, .imm=2});
		asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=0});
	}
}

// TODO
void LtNode::computeConst() {
	// Signed vs. Unsigned comparison.
	if (m_varType == TYPE_UINT ||
		m_varType == TYPE_UCHAR) { // Unsigned comparison
		m_constVal = (uint16_t)(m_lhs->getConstVal()) <
			(uint16_t)(m_rhs->getConstVal());
	}
	else { // Signed comparison
		m_constVal = (int16_t)(m_lhs->getConstVal()) <
			(int16_t)(m_rhs->getConstVal());
	}
	m_constVal &= 0xffff;
}

// TODO
void LtNode::genExp(AsmMaker* asmGen, RegName_e lhs, RegName_e rhs) {
	// Move current result as needed to generate new one.
	string cmt = "Move operand for result processing";
	if (lhs == REG_AC) {
		asmGen->genInstr({.type=INSTR_ADD, .r1=REG_RA,
			.r2=lhs, .imm=0, .cmt=cmt});
		lhs = REG_RA;
	}
	else if (rhs == REG_AC) {
		asmGen->genInstr({.type=INSTR_ADD, .r1=REG_RA,
			.r2=rhs, .imm=0, .cmt=cmt});
		rhs = REG_RA;
	}

	// Signed vs. Unsigned comparison.
	if (m_varType == TYPE_UINT ||
		m_varType == TYPE_UCHAR) { // Unsigned comparison
		// Generate unsigned check.
		asmGen->genInstr({.type=INSTR_XOR, .r1=REG_AC, .r2=lhs, .r3=rhs});
		asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_N, .imm=10});
		asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=0});
		asmGen->genInstr({.type=INSTR_SUB, .r1=REG_RA, .r2=rhs, .r3=lhs});
		asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_ZP, .imm=8});
		asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=1});
		asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_NZP, .imm=4});
		asmGen->genInstr({.type=INSTR_AND, .r1=REG_AC, .r2=REG_AC, .r3=rhs});
		asmGen->genInstr({.type=INSTR_SHR, .r1=REG_AC, .r2=REG_AC, .imm=15});
	}
	else { // Signed comparison
		// Generate signed check.
		asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=1});
		asmGen->genInstr({.type=INSTR_SUB, .r1=REG_RA, .r2=rhs, .r3=lhs});
		asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_N, .imm=2});
		asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=0});
	}
}

// TODO
void GeqNode::computeConst() {
	// Signed vs. Unsigned comparison.
	if (m_varType == TYPE_UINT ||
		m_varType == TYPE_UCHAR) { // Unsigned comparison
		m_constVal = (uint16_t)(m_lhs->getConstVal()) >=
			(uint16_t)(m_rhs->getConstVal());
	}
	else { // Signed comparison
		m_constVal = (int16_t)(m_lhs->getConstVal()) >=
			(int16_t)(m_rhs->getConstVal());
	}
	m_constVal &= 0xffff;
}

// TODO
void GeqNode::genExp(AsmMaker* asmGen, RegName_e lhs, RegName_e rhs) {
	// Move current result as needed to generate new one.
	string cmt = "Move operand for result processing";
	if (lhs == REG_AC) {
		asmGen->genInstr({.type=INSTR_ADD, .r1=REG_RA,
			.r2=lhs, .imm=0, .cmt=cmt});
		lhs = REG_RA;
	}
	else if (rhs == REG_AC) {
		asmGen->genInstr({.type=INSTR_ADD, .r1=REG_RA,
			.r2=rhs, .imm=0, .cmt=cmt});
		rhs = REG_RA;
	}

	// Signed vs. Unsigned comparison.
	if (m_varType == TYPE_UINT ||
		m_varType == TYPE_UCHAR) { // Unsigned comparison
		// Generate unsigned check.
		asmGen->genInstr({.type=INSTR_XOR, .r1=REG_AC, .r2=lhs, .r3=rhs});
		asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_N, .imm=10});
		asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=0});
		asmGen->genInstr({.type=INSTR_SUB, .r1=REG_RA, .r2=lhs, .r3=rhs});
		asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_P, .imm=8});
		asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=1});
		asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_NZP, .imm=4});
		asmGen->genInstr({.type=INSTR_AND, .r1=REG_AC, .r2=REG_AC, .r3=lhs});
		asmGen->genInstr({.type=INSTR_SHR, .r1=REG_AC, .r2=REG_AC, .imm=15});
	}
	else { // Signed comparison
		// Generate signed check.
		asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=1});
		asmGen->genInstr({.type=INSTR_SUB, .r1=REG_RA, .r2=lhs, .r3=rhs});
		asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_NZ, .imm=2});
		asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=0});
	}
}

// TODO
void LeqNode::computeConst() {
	// Signed vs. Unsigned comparison.
	if (m_varType == TYPE_UINT ||
		m_varType == TYPE_UCHAR) { // Unsigned comparison
		m_constVal = (uint16_t)(m_lhs->getConstVal()) <=
			(uint16_t)(m_rhs->getConstVal());
	}
	else { // Signed comparison
		m_constVal = (int16_t)(m_lhs->getConstVal()) <=
			(int16_t)(m_rhs->getConstVal());
	}
	m_constVal &= 0xffff;
}

// TODO
void LeqNode::genExp(AsmMaker* asmGen, RegName_e lhs, RegName_e rhs) {
	// Move current result as needed to generate new one.
	string cmt = "Move operand for result processing";
	if (lhs == REG_AC) {
		asmGen->genInstr({.type=INSTR_ADD, .r1=REG_RA,
			.r2=lhs, .imm=0, .cmt=cmt});
		lhs = REG_RA;
	}
	else if (rhs == REG_AC) {
		asmGen->genInstr({.type=INSTR_ADD, .r1=REG_RA,
			.r2=rhs, .imm=0, .cmt=cmt});
		rhs = REG_RA;
	}

	// Signed vs. Unsigned comparison.
	if (m_varType == TYPE_UINT ||
		m_varType == TYPE_UCHAR) { // Unsigned comparison
		// Generate unsigned check.
		asmGen->genInstr({.type=INSTR_XOR, .r1=REG_AC, .r2=lhs, .r3=rhs});
		asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_N, .imm=10});
		asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=0});
		asmGen->genInstr({.type=INSTR_SUB, .r1=REG_RA, .r2=rhs, .r3=lhs});
		asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_P, .imm=8});
		asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=1});
		asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_NZP, .imm=4});
		asmGen->genInstr({.type=INSTR_AND, .r1=REG_AC, .r2=REG_AC, .r3=rhs});
		asmGen->genInstr({.type=INSTR_SHR, .r1=REG_AC, .r2=REG_AC, .imm=15});
	}
	else { // Signed comparison
		// Generate signed check.
		asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=1});
		asmGen->genInstr({.type=INSTR_SUB, .r1=REG_RA, .r2=rhs, .r3=lhs});
		asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_NZ, .imm=2});
		asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=0});
	}
}

// TODO
void EqNode::computeConst() {
	// Signed vs. Unsigned comparison.
	if (m_varType == TYPE_UINT ||
		m_varType == TYPE_UCHAR) { // Unsigned comparison
		m_constVal = (uint16_t)(m_lhs->getConstVal()) ==
			(uint16_t)(m_rhs->getConstVal());
	}
	else { // Signed comparison
		m_constVal = (int16_t)(m_lhs->getConstVal()) ==
			(int16_t)(m_rhs->getConstVal());
	}
	m_constVal &= 0xffff;
}

// TODO
void EqNode::genExp(AsmMaker* asmGen, RegName_e lhs, RegName_e rhs) {
	// Move current result as needed to generate new one.
	string cmt = "Move operand for result processing";
	if (lhs == REG_AC) {
		asmGen->genInstr({.type=INSTR_ADD, .r1=REG_RA,
			.r2=lhs, .imm=0, .cmt=cmt});
		lhs = REG_RA;
	}
	else if (rhs == REG_AC) {
		asmGen->genInstr({.type=INSTR_ADD, .r1=REG_RA,
			.r2=rhs, .imm=0, .cmt=cmt});
		rhs = REG_RA;
	}

	asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=1});
	asmGen->genInstr({.type=INSTR_SUB, .r1=REG_RA, .r2=lhs, .r3=rhs});
	asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_Z, .imm=2});
	asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=0});
}

// TODO
void NeqNode::computeConst() {
	// Signed vs. Unsigned comparison.
	if (m_varType == TYPE_UINT ||
		m_varType == TYPE_UCHAR) { // Unsigned comparison
		m_constVal = (uint16_t)(m_lhs->getConstVal()) !=
			(uint16_t)(m_rhs->getConstVal());
	}
	else { // Signed comparison
		m_constVal = (int16_t)(m_lhs->getConstVal()) !=
			(int16_t)(m_rhs->getConstVal());
	}
	m_constVal &= 0xffff;
}

// TODO
void NeqNode::genExp(AsmMaker* asmGen, RegName_e lhs, RegName_e rhs) {
	asmGen->genInstr({.type=INSTR_SUB, .r1=REG_AC, .r2=lhs, .r3=rhs});
	asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_Z, .imm=2});
	asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=1});
}

// TODO
void BNotNode::computeConst() {
	m_constVal = !m_rhs->getConstVal();
	m_constVal &= 0xffff;
}

// TODO
void BNotNode::genExp(AsmMaker* asmGen, RegName_e lhs, RegName_e rhs) {
	// Move current result as needed to generate new one.
	string cmt = "Move operand for result processing";
	if (lhs == REG_AC) {
		asmGen->genInstr({.type=INSTR_ADD, .r1=REG_RA,
			.r2=lhs, .imm=0, .cmt=cmt});
		lhs = REG_RA;
	}
	else if (rhs == REG_AC) {
		asmGen->genInstr({.type=INSTR_ADD, .r1=REG_RA,
			.r2=rhs, .imm=0, .cmt=cmt});
		rhs = REG_RA;
	}

	string ccCmt = "Get condition codes";
	asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=0});
	asmGen->genInstr({.type=INSTR_ADD, .r1=REG_RA,
		.r2=rhs, .imm=0, .cmt=ccCmt});
	asmGen->genInstr({.type=INSTR_BRC, .flg=FLAG_NP, .imm=2});
	asmGen->genInstr({.type=INSTR_LBI, .r1=REG_AC, .imm=1});
}
