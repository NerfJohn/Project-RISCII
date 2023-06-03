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

private:
	// TODO
	std::vector<IDeclNode*> m_declList;
};

/////////////////////////
// === VarDeclNode === //
/////////////////////////

// TODO
class VarDeclNode: public IASTNode, public IDeclNode, public IStmtNode {
public:
	// TODO
	VarDeclNode(std::stack<IBuildItem*>* buildStack);

	// TODO
	uint8_t getBuildType(void) {return PARSE_VDECL_NODE;}
	//int getLineNum(void);
	std::string getValue(void) {return m_id;}

	// TODO
	std::string toString(void);
};

/////////////////////////
// === FuncDefNode === //
/////////////////////////

// TODO
class FuncDefNode: public IASTNode, public IDeclNode {
public:
	// TODO
	FuncDefNode(std::stack<IBuildItem*>* buildStack);

	// TODO
	uint8_t getBuildType(void) {return PARSE_FDEF_NODE;}
	//int getLineNum(void);
	std::string getValue(void) {return m_id;}

	// TODO
	std::string toString(void);

private:
	// TODO
	std::vector<IDeclNode*> m_paramList;
	std::vector<IStmtNode*> m_stmtList;
};

////////////////////
// === IDNode === //
////////////////////

// TODO
class IDNode: public IASTNode, public IExpNode {
public:
	// TODO
	IDNode(std::stack<IBuildItem*>* buildStack);

	// TODO
	uint8_t getBuildType(void) {return PARSE_ID_NODE;}
	//int getLineNum(void);
	std::string getValue(void) {return m_id;}

	// TODO
	std::string toString(void);

private:
	// TODO
	std::string m_id;

};

/////////////////////
// === LITNode === //
/////////////////////

// TODO
class LITNode: public IASTNode, public IExpNode {
public:
	// TODO
	LITNode(std::stack<IBuildItem*>* buildStack);

	// TODO
	uint8_t getBuildType(void) {return PARSE_LIT_NODE;}
	//int getLineNum(void);
	std::string getValue(void) {return m_strValue;}

	// TODO
	std::string toString(void);

private:
	// TODO
	std::string m_strValue;
};

////////////////////////
// === AssignNode === //
////////////////////////

// TODO
class AssignNode: public IASTNode, public IStmtNode {
public:
	// TODO
	AssignNode(std::stack<IBuildItem*>* buildStack);

	// TODO
	uint8_t getBuildType(void) {return PARSE_ASSIGN_NODE;}
	//int getLineNum(void);
	//std::string getValue(void);

	// TODO
	std::string toString(void);

private:
	// TODO
	IDNode* m_lhs;
	IExpNode* m_rhs;
};

/////////////////////
// === RetNode === //
/////////////////////

// TODO
class RetNode: public IASTNode, public IStmtNode {
public:
	// TODO
	RetNode(std::stack<IBuildItem*>* buildStack);

	// TODO
	uint8_t getBuildType(void) {return PARSE_RETURN_NODE;}
	//int getLineNum(void);
	//std::string getValue(void);

	// TODO
	std::string toString(void);

private:
	// TODO
	IExpNode* m_exp;
};

////////////////////
// === IfNode === //
////////////////////

// TODO
class IfNode: public IASTNode, public IStmtNode {
public:
	// TODO
	IfNode(std::stack<IBuildItem*>* buildStack);

	// TODO
	uint8_t getBuildType(void) {return PARSE_IF_NODE;}
	//int getLineNum(void);
	//std::string getValue(void);

	// TODO
	std::string toString(void);

private:
	// TODO
	IExpNode* m_cond;
	std::vector<IStmtNode*> m_stmtList;
};

///////////////////////
// === WhileNode === //
///////////////////////

// TODO
class WhileNode: public IASTNode, public IStmtNode {
public:
	// TODO
	WhileNode(std::stack<IBuildItem*>* buildStack);

	// TODO
	uint8_t getBuildType(void) {return PARSE_WHILE_NODE;}
	//int getLineNum(void);
	//std::string getValue(void);

	// TODO
	std::string toString(void);

private:
	// TODO
	IExpNode* m_cond;
	std::vector<IStmtNode*> m_stmtList;
};

//////////////////////
// === CallNode === //
//////////////////////

// TODO
class CallNode: public IASTNode, public IStmtNode, public IExpNode {
public:
	// TODO
	CallNode(std::stack<IBuildItem*>* buildStack);

	// TODO
	uint8_t getBuildType(void) {return PARSE_CALL_NODE;}
	//int getLineNum(void);
	//std::string getValue(void);

	// TODO
	std::string toString(void);

private:
	// TODO
	IDNode* m_id;
	std::vector<IExpNode*> m_argList;
};

///////////////////////////
// === Logical Nodes === //
///////////////////////////

// TODO
class AndNode: public IASTNode, public IExpNode {
public:
	// TODO
	AndNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_AND_NODE;}

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class OrNode: public IASTNode, public IExpNode {
public:
	// TODO
	OrNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_OR_NODE;}

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class XorNode: public IASTNode, public IExpNode {
public:
	// TODO
	XorNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_XOR_NODE;}

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class LNotNode: public IASTNode, public IExpNode {
public:
	// TODO
	LNotNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_LNOT_NODE;}

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class RShiftNode: public IASTNode, public IExpNode {
public:
	// TODO
	RShiftNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_RSHIFT_NODE;}

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class LShiftNode: public IASTNode, public IExpNode {
public:
	// TODO
	LShiftNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_LSHIFT_NODE;}

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

//////////////////////////////
// === Arithmetic Nodes === //
//////////////////////////////

// TODO
class PlusNode: public IASTNode, public IExpNode {
public:
	// TODO
	PlusNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_PLUS_NODE;}

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class MinusNode: public IASTNode, public IExpNode {
public:
	// TODO
	MinusNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_MINUS_NODE;}

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

///////////////////////////
// === Boolean Nodes === //
///////////////////////////

// TODO
class GrtNode: public IASTNode, public IExpNode {
public:
	// TODO
	GrtNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_GRT_NODE;}

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class LtNode: public IASTNode, public IExpNode {
public:
	// TODO
	LtNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_LT_NODE;}

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class GeqNode: public IASTNode, public IExpNode {
public:
	// TODO
	GeqNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_GEQ_NODE;}

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class LeqNode: public IASTNode, public IExpNode {
public:
	// TODO
	LeqNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_LEQ_NODE;}

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class EqNode: public IASTNode, public IExpNode {
public:
	// TODO
	EqNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_EQ_NODE;}

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class NeqNode: public IASTNode, public IExpNode {
public:
	// TODO
	NeqNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_NEQ_NODE;}

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

// TODO
class BNotNode: public IASTNode, public IExpNode {
public:
	// TODO
	BNotNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// TODO
	uint8_t getBuildType(void) {return PARSE_BNOT_NODE;}

	// TODO
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}
};

#endif /* NODECONCRETES_H_ */
