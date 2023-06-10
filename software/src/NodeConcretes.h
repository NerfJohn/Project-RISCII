/*
 * NodeConcretes.h
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

#ifndef NODECONCRETES_H_
#define NODECONCRETES_H_

#include <stack>
#include <vector>
#include "NodeAbstracts.h"
#include "ParseTable.h"

//////////////////////
// === PrgmNode === //
//////////////////////

// TODO
class PrgmNode: public IASTNode {
public:
	// TODO
	PrgmNode(std::stack<IBuildItem*>* buildStack);

	// TODO
	uint8_t getBuildType(void) {return PARSE_PRGM_NODE;}
	//int getLineNum(void);
	//std::string getValue(void);

	// TODO
	std::string toString(void);

	// TODO
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList);

	// TODO
	VarType_e checkTyping(void);

	// TODO
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);

private:
	// TODO
	std::vector<IDeclNode*> m_declList;
};

/////////////////////////
// === VarDeclNode === //
/////////////////////////

// TODO
class VarDeclNode: public IDeclNode, public IStmtNode {
public:
	// TODO
	VarDeclNode(std::stack<IBuildItem*>* buildStack);

	// TODO
	uint8_t getBuildType(void) {return PARSE_VDECL_NODE;}
	//int getLineNum(void);
	std::string getValue(void) {return m_id;}

	// TODO
	std::string toString(void);

	// TODO
	void initParam(void) {m_isParam = true;}

	// TODO
	VarType_e getType(void) {return m_varType;}

	// TODO
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList);

	// TODO
	VarType_e checkTyping(void) {/* no actions */ return (VarType_e)(-1);}

	// TODO
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);

private:
	// TODO
	bool m_isParam = false;
};

/////////////////////////
// === FuncDefNode === //
/////////////////////////

// TODO
class FuncDefNode: public IDeclNode {
public:
	// TODO
	FuncDefNode(std::stack<IBuildItem*>* buildStack);

	// TODO
	uint8_t getBuildType(void) {return PARSE_FDEF_NODE;}
	//int getLineNum(void);
	std::string getValue(void) {return m_id;}

	// TODO
	std::string toString(void);

	// TODO
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList);

	// TODO
	VarType_e checkTyping(void);

	// TODO
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);

	// TODO
	~FuncDefNode(void);

private:
	// TODO
	std::vector<VarDeclNode*> m_paramList;
	std::vector<IStmtNode*> m_stmtList;

	// TODO
	bool m_unreachTriggered = false;
};

////////////////////
// === IDNode === //
////////////////////

// TODO
class IDNode: public IExpNode {
public:
	// TODO
	IDNode(std::stack<IBuildItem*>* buildStack);

	// TODO
	uint8_t getBuildType(void) {return PARSE_ID_NODE;}
	//int getLineNum(void);
	std::string getValue(void) {return m_id;}

	// TODO
	std::string toString(void);

	// TODO
	void initID(void) {m_isInit = true;}

	// TODO
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList);

	// TODO
	VarType_e checkTyping(void);

	// TODO
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);
	void computeConst(void) {/* No actions */}

	// TODO
	Symbol* getSym(void) {return m_sym;}

private:
	// TODO
	std::string m_id;

	// TODO
	bool m_isInit = false;

	// TODO
	Symbol* m_sym;
};

/////////////////////
// === LITNode === //
/////////////////////

// TODO
class LITNode: public IExpNode {
public:
	// TODO
	LITNode(std::stack<IBuildItem*>* buildStack);

	// TODO
	uint8_t getBuildType(void) {return PARSE_LIT_NODE;}
	//int getLineNum(void);
	std::string getValue(void) {return m_strValue;}

	// TODO
	std::string toString(void);

	// TODO
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList) {
		/* no work to do */
	}

	// TODO
	VarType_e checkTyping(void);

	// TODO
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);
	void computeConst(void) {/* No actions */}

	// TODO
	void negateInt(void) {m_negateInt = true;}

private:
	// TODO
	std::string m_strValue;

	// TODO
	bool m_negateInt = false;
};

////////////////////////
// === AssignNode === //
////////////////////////

// TODO
class AssignNode: public IStmtNode {
public:
	// TODO
	AssignNode(std::stack<IBuildItem*>* buildStack);

	// TODO
	uint8_t getBuildType(void) {return PARSE_ASSIGN_NODE;}
	//int getLineNum(void);
	//std::string getValue(void);

	// TODO
	std::string toString(void);

	// TODO
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList);

	// TODO
	VarType_e checkTyping(void);

	// TODO
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);

	// TODO
	~AssignNode(void);

private:
	// TODO
	IDNode* m_lhs;
	IExpNode* m_rhs;
};

/////////////////////
// === RetNode === //
/////////////////////

// TODO
class RetNode: public IStmtNode {
public:
	// TODO
	RetNode(std::stack<IBuildItem*>* buildStack);

	// TODO
	uint8_t getBuildType(void) {return PARSE_RETURN_NODE;}
	//int getLineNum(void);
	//std::string getValue(void);

	// TODO
	std::string toString(void);

	// TODO
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList);

	// TODO
	VarType_e checkTyping(void);

	// TODO
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);

	// TODO
	~RetNode(void);

private:
	// TODO
	IExpNode* m_exp;
};

////////////////////
// === IfNode === //
////////////////////

// TODO
class IfNode: public IStmtNode {
public:
	// TODO
	IfNode(std::stack<IBuildItem*>* buildStack);

	// TODO
	uint8_t getBuildType(void) {return PARSE_IF_NODE;}
	//int getLineNum(void);
	//std::string getValue(void);

	// TODO
	std::string toString(void);

	// TODO
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList);

	// TODO
	VarType_e checkTyping(void);

	// TODO
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);

	// TODO
	~IfNode(void);

private:
	// TODO
	IExpNode* m_cond;
	std::vector<IStmtNode*> m_stmtList;

	// TODO
	bool m_unreachTriggered = false;
};

///////////////////////
// === WhileNode === //
///////////////////////

// TODO
class WhileNode: public IStmtNode {
public:
	// TODO
	WhileNode(std::stack<IBuildItem*>* buildStack);

	// TODO
	uint8_t getBuildType(void) {return PARSE_WHILE_NODE;}
	//int getLineNum(void);
	//std::string getValue(void);

	// TODO
	std::string toString(void);

	// TODO
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList);

	// TODO
	VarType_e checkTyping(void);

	// TODO
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);

	// TODO
	~WhileNode(void);

private:
	// TODO
	IExpNode* m_cond;
	std::vector<IStmtNode*> m_stmtList;

	// TODO
	bool m_unreachTriggered = false;
};

//////////////////////
// === CallNode === //
//////////////////////

// TODO
class CallNode: public IStmtNode, public IExpNode {
public:
	// TODO
	CallNode(std::stack<IBuildItem*>* buildStack);

	// TODO
	uint8_t getBuildType(void) {return PARSE_CALL_NODE;}
	//int getLineNum(void);
	//std::string getValue(void);

	// TODO
	std::string toString(void);

	// TODO
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList);

	// TODO
	VarType_e checkTyping(void);

	// TODO
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);
	void computeConst(void) {/* No actions */}

	// TODO
	~CallNode(void);

private:
	// TODO
	IDNode* m_id;
	std::vector<IExpNode*> m_argList;

	// TODO
	Symbol* m_sym;
};

///////////////////////////
// === Logical Nodes === //
///////////////////////////

// TODO
class AndNode: public IExpNode {
public:
	// TODO
	AndNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_AND_NODE;}

	// TODO
	void computeConst(void);

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class OrNode: public IExpNode {
public:
	// TODO
	OrNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_OR_NODE;}

	// TODO
	void computeConst(void);

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class XorNode: public IExpNode {
public:
	// TODO
	XorNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_XOR_NODE;}

	// TODO
	void computeConst(void);

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class LNotNode: public IExpNode {
public:
	// TODO
	LNotNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_LNOT_NODE;}

	// TODO
	void computeConst(void);

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class RShiftNode: public IExpNode {
public:
	// TODO
	RShiftNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_RSHIFT_NODE;}

	// TODO
	void computeConst(void);

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class LShiftNode: public IExpNode {
public:
	// TODO
	LShiftNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_LSHIFT_NODE;}

	// TODO
	void computeConst(void);

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

//////////////////////////////
// === Arithmetic Nodes === //
//////////////////////////////

// TODO
class PlusNode: public IExpNode {
public:
	// TODO
	PlusNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_PLUS_NODE;}

	// TODO
	void computeConst(void);

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class MinusNode: public IExpNode {
public:
	// TODO
	MinusNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_MINUS_NODE;}

	// TODO
	void computeConst(void);

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}

	// TODO
	VarType_e checkTyping(void);
};

///////////////////////////
// === Boolean Nodes === //
///////////////////////////

// TODO
class GrtNode: public IExpNode {
public:
	// TODO
	GrtNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_GRT_NODE;}

	// TODO
	void computeConst(void);

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class LtNode: public IExpNode {
public:
	// TODO
	LtNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_LT_NODE;}

	// TODO
	void computeConst(void);

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class GeqNode: public IExpNode {
public:
	// TODO
	GeqNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_GEQ_NODE;}

	// TODO
	void computeConst(void);

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class LeqNode: public IExpNode {
public:
	// TODO
	LeqNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_LEQ_NODE;}

	// TODO
	void computeConst(void);

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class EqNode: public IExpNode {
public:
	// TODO
	EqNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_EQ_NODE;}

	// TODO
	void computeConst(void);

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class NeqNode: public IExpNode {
public:
	// TODO
	NeqNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_NEQ_NODE;}

	// TODO
	void computeConst(void);

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class BNotNode: public IExpNode {
public:
	// TODO
	BNotNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_BNOT_NODE;}

	// TODO
	void computeConst(void);

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

#endif /* NODECONCRETES_H_ */
