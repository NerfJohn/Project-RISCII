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
IDeclNode* IASTNode::m_curFunc = nullptr;

// TODO
int IASTNode::m_numDeletes = 0;

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

// TODO
VarType_e IASTNode::getNewTyping(VarType_e lhs, VarType_e rhs, int lineNum) {
	// "TYPE_LITERAL" short-circuits to other option.
	if (lhs == TYPE_LITERAL) {return rhs;}
	if (rhs == TYPE_LITERAL) {return lhs;}

	// Homogeneous types produce same type.
	if (lhs == rhs) {return lhs; /* or rhs, arbitrary decision */}

	// Uints supercedes all.
	if ((lhs == TYPE_UINT) || (rhs == TYPE_UINT)) {
		// Report potential loss if other side is signed.
		VarType_e otherType = (lhs == TYPE_UINT) ? rhs : lhs;
		if ((otherType == TYPE_INT) || (otherType == TYPE_CHAR)) {
			MsgLog::logWARN(VarType::toString(lhs) + " and " +
					VarType::toString(rhs) + " creates a UINT- may be lossy",
					lineNum);
		}

		// Otherwise, typing will be UINT.
		return TYPE_UINT;
	}

	// All remaining cases between INT, CHAR, and UCHAR result in INT.
	return TYPE_INT;
}

// TODO
bool IASTNode::canAssignTyping(VarType_e goal, VarType_e start) {
	// Literals and same-same scenarios always work.
	if ((goal == TYPE_LITERAL) ||
		(start == TYPE_LITERAL) ||
		(goal == start)) {
		return true;
	}

	// Process each remaining possibility.
	switch (goal) {
		case TYPE_UINT: return start == TYPE_UCHAR;
		case TYPE_INT:	return start != TYPE_UINT; // ie char/uchar are good
		default: // CHAR and UCHAR
			return false; // Not same or literal = will require a warning
	}
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

// TODO
VarType_e IExpNode::checkTyping(void) {
	// Pass to children (as available).
	VarType_e lhsType= TYPE_LITERAL;
	VarType_e rhsType = TYPE_LITERAL;
	if (m_lhs != nullptr) {lhsType = m_lhs->checkTyping();}
	if (m_rhs != nullptr) {rhsType = m_rhs->checkTyping();}

	// Determine new typing to pass up.
	m_varType = IASTNode::getNewTyping(lhsType, rhsType, m_lineNum);

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
int IExpNode::optimizeAST(std::unordered_map<Symbol*,int>* constList) {
	// Optimize children (treat nullptrs as "constant nothings").
	int lhsValue = OPT_CONST_MAX;
	int rhsValue = OPT_CONST_MAX;
	if (m_lhs != nullptr) {lhsValue = m_lhs->optimizeAST(constList);}
	if (m_rhs != nullptr) {rhsValue = m_rhs->optimizeAST(constList);}

	// Set/return as constant if children are constant.
	if (lhsValue <= OPT_CONST_MAX &&
		rhsValue <= OPT_CONST_MAX) {
		// Computer constant.
		this->computeConst();

		string type =
				IASTNode::nodeToString((ParseActions)this->getBuildType());
		MsgLog::logINFO(type + " at " + to_string(m_lineNum)
				+ " now a const " + to_string(m_constVal));

		// Prune children.
		if (m_lhs != nullptr) {delete m_lhs; m_lhs = nullptr;}
		if (m_rhs != nullptr) {delete m_rhs; m_rhs = nullptr;}

		// Return computed value.
		return m_constVal;
	}

	// Can't be optimized- keep expression.
	return OPT_VAL_UNKNOWN;
}

// TODO
IExpNode::~IExpNode(void) {
	// Delete children.
	if (m_lhs != nullptr) {delete m_lhs; m_lhs = nullptr;}
	if (m_rhs != nullptr) {delete m_rhs; m_rhs = nullptr;}
}
