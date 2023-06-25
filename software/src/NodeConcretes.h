/*
 * NodeConcretes.h
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

// Node representing an entire program/file.
class PrgmNode: public IASTNode {
public:
	// Constructor- builds node from build stack during parsing.
	PrgmNode(std::stack<IBuildItem*>* buildStack);

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_PRGM_NODE;}
	//int getLineNum(void);
	//std::string getValue(void);

	// Required form of toString().
	std::string toString(void);

	// Required implementation to conduct semantic analysis on the AST.
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList);

	// Required implementation to conduct type checking on the AST.
	VarType_e checkTyping(void);

	// Required implementation to conduct optimizations of the AST.
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);

	// Required implementation to conduct translation of the AST.
	genValue_t translate(AsmMaker* asmGen);

private:
	// Ordered list of global declarations makign up the program.
	std::vector<IDeclNode*> m_declList;
};

/////////////////////////
// === VarDeclNode === //
/////////////////////////

// Node representing a variable declaration (statement or otherwise).
class VarDeclNode: public IDeclNode, public IStmtNode {
public:
	// Constructor- builds node from build stack during parsing.
	VarDeclNode(std::stack<IBuildItem*>* buildStack);

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_VDECL_NODE;}
	//int getLineNum(void);
	std::string getValue(void) {return m_id;}

	// Required form of toString().
	std::string toString(void);

	// Required implementation to conduct semantic analysis on the AST.
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList);

	// Required implementation to conduct type checking on the AST.
	VarType_e checkTyping(void) {/* no actions */ return (VarType_e)(-1);}

	// Required implementation to conduct optimizations of the AST.
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);

	// Required implementation to conduct translation of the AST.
	genValue_t translate(AsmMaker* asmGen) {/* irrelevant */ return {};}

	// Helper function to mark declared variable as parameter.
	void initParam(void) {m_isParam = true;}

	// Helper function to get the variable's return type.
	VarType_e getType(void) {return m_varType;}

private:
	// Keeps track of variable being a parameter or not- usually not.
	bool m_isParam = false;
};

/////////////////////////
// === FuncDefNode === //
/////////////////////////

// Node representing a function definition.
class FuncDefNode: public IDeclNode {
public:
	// Next open local address/frame offset for this function.
	int m_nextLocalAddr = -2; // Locals start just under the FP

	// Constructor- builds node from build stack during parsing.
	FuncDefNode(std::stack<IBuildItem*>* buildStack);

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_FDEF_NODE;}
	//int getLineNum(void);
	std::string getValue(void) {return m_id;}

	// Required form of toString().
	std::string toString(void);

	// Required implementation to conduct semantic analysis on the AST.
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList);

	// Required implementation to conduct type checking on the AST.
	VarType_e checkTyping(void);

	// Required implementation to conduct optimizations of the AST.
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);

	// Required implementation to conduct translation of the AST.
	genValue_t translate(AsmMaker* asmGen);

	// Function to check is a given statement is the last one in the function.
	bool isLastStmt(IStmtNode* stmt);

	// Destructor to delete children.
	~FuncDefNode(void);

private:
	// Ordered lists of stored parameters and statements.
	std::vector<VarDeclNode*> m_paramList;
	std::vector<IStmtNode*> m_stmtList;

	// Keeps track if a return has been found at the function's scope.
	bool m_unreachTriggered = false;
};

////////////////////
// === IDNode === //
////////////////////

// Node representing a use of an identifier/ID.
class IDNode: public IExpNode {
public:
	// Constructor- builds node from build stack during parsing.
	IDNode(std::stack<IBuildItem*>* buildStack);

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_ID_NODE;}
	//int getLineNum(void);
	std::string getValue(void) {return m_id;}

	// Required form of toString().
	std::string toString(void);

	// Required implementation to conduct semantic analysis on the AST.
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList);

	// Required implementation to conduct type checking on the AST.
	VarType_e checkTyping(void);

	// Required implementation to conduct optimizations of the AST.
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);

	// Required implementation to conduct translation of the AST.
	genValue_t translate(AsmMaker* asmGen);

	// Getter for ID's linked symbol.
	Symbol* getSym(void) {return m_sym;}

	// Helper function to mark ID as initialized.
	void initID(void) {m_isInit = true;}

	// Required implementation to compute an ExpNode's specific constant.
	void computeConst(void) {/* No actions */}

	// Required implementation to translate an ExpNode's specific kind.
	void genExp(AsmMaker*, RegName_e lhs, RegName_e rhs) {return;}

private:
	// ID's stored name.
	std::string m_id;

	// Flag to keep track if has been initialized.
	bool m_isInit = false;

	// Linked symbol with ID's larger information.
	Symbol* m_sym;
};

/////////////////////
// === LITNode === //
/////////////////////

// Node representing a use of a literal value.
class LITNode: public IExpNode {
public:
	// Constructor- builds node from build stack during parsing.
	LITNode(std::stack<IBuildItem*>* buildStack);

	// Constructor- builds node from a provided literal value.
	LITNode(int value);

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_LIT_NODE;}
	//int getLineNum(void);
	std::string getValue(void) {return m_strValue;}

	// Required form of toString().
	std::string toString(void);

	// Required implementation to conduct semantic analysis on the AST.
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList) {
		/* no work to do */
	}

	// Required implementation to conduct type checking on the AST.
	VarType_e checkTyping(void);

	// Required implementation to conduct optimizations of the AST.
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);

	// Required implementation to conduct translation of the AST.
	genValue_t translate(AsmMaker* asmGen);

	// Mark the stored value as negated (by an outside unary minus).
	void negateInt(void) {m_negateInt = true;}

	// Required implementation to compute an ExpNode's specific constant.
	void computeConst(void) {/* No actions */}

	// Required implementation to translate an ExpNode's specific kind.
	void genExp(AsmMaker*, RegName_e lhs, RegName_e rhs) {return;}

private:
	// Original string value of integer value being stored.
	std::string m_strValue;

	// Flag to keep track if internal value should be negated.
	bool m_negateInt = false;
};

////////////////////////
// === AssignNode === //
////////////////////////

// Node representing an assign statement.
class AssignNode: public IStmtNode {
public:
	// Constructor- builds node from build stack during parsing.
	AssignNode(std::stack<IBuildItem*>* buildStack);

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_ASSIGN_NODE;}
	//int getLineNum(void);
	//std::string getValue(void);

	// Required form of toString().
	std::string toString(void);

	// Required implementation to conduct semantic analysis on the AST.
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList);

	// Required implementation to conduct type checking on the AST.
	VarType_e checkTyping(void);

	// Required implementation to conduct optimizations of the AST.
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);

	// Required implementation to conduct translation of the AST.
	genValue_t translate(AsmMaker* asmGen);

	// Destructor to delete children.
	~AssignNode(void);

private:
	// Links to both sides/sub-trees of the assignment.
	IDNode* m_lhs;
	IExpNode* m_rhs;
};

/////////////////////
// === RetNode === //
/////////////////////

// Node representing a return statement.
class RetNode: public IStmtNode {
public:
	// Constructor- builds node from build stack during parsing.
	RetNode(std::stack<IBuildItem*>* buildStack);

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_RETURN_NODE;}
	//int getLineNum(void);
	//std::string getValue(void);

	// Required form of toString().
	std::string toString(void);

	// Required implementation to conduct semantic analysis on the AST.
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList);

	// Required implementation to conduct type checking on the AST.
	VarType_e checkTyping(void);

	// Required implementation to conduct optimizations of the AST.
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);

	// Required implementation to conduct translation of the AST.
	genValue_t translate(AsmMaker* asmGen);

	// Destructor to delete children.
	~RetNode(void);

private:
	// Link to sub-tree containing the returned expression.
	IExpNode* m_exp;
};

////////////////////
// === IfNode === //
////////////////////

// Node representing an "if" statement.
class IfNode: public IStmtNode {
public:
	// Constructor- builds node from build stack during parsing.
	IfNode(std::stack<IBuildItem*>* buildStack);

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_IF_NODE;}
	//int getLineNum(void);
	//std::string getValue(void);

	// Required form of toString().
	std::string toString(void);

	// Required implementation to conduct semantic analysis on the AST.
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList);

	// Required implementation to conduct type checking on the AST.
	VarType_e checkTyping(void);

	// Required implementation to conduct optimizations of the AST.
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);

	// Required implementation to conduct translation of the AST.
	genValue_t translate(AsmMaker* asmGen);

	// Destructor to delete children.
	~IfNode(void);

private:
	// Link to expression providing the condition.
	IExpNode* m_cond;

	// Ordered list of statements within the block of the if.
	std::vector<IStmtNode*> m_stmtList;

	// Flag to mark a return has been found within the block's scope.
	bool m_unreachTriggered = false;
};

///////////////////////
// === WhileNode === //
///////////////////////

// Node representing a "while" statement.
class WhileNode: public IStmtNode {
public:
	// Constructor- builds node from build stack during parsing.
	WhileNode(std::stack<IBuildItem*>* buildStack);

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_WHILE_NODE;}
	//int getLineNum(void);
	//std::string getValue(void);

	// Required form of toString().
	std::string toString(void);

	// Required implementation to conduct semantic analysis on the AST.
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList);

	// Required implementation to conduct type checking on the AST.
	VarType_e checkTyping(void);

	// Required implementation to conduct optimizations of the AST.
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);

	// Required implementation to conduct translation of the AST.
	genValue_t translate(AsmMaker* asmGen);

	// Destructor to delete children.
	~WhileNode(void);

private:
	// Link to expression providing the condition.
	IExpNode* m_cond;

	// Ordered list of statements within the block of the while.
	std::vector<IStmtNode*> m_stmtList;

	// Flag to mark a return has been found within the block's scope.
	bool m_unreachTriggered = false;
};

//////////////////////
// === CallNode === //
//////////////////////

// Node representing a call to a function.
class CallNode: public IStmtNode, public IExpNode {
public:
	// Constructor- builds node from build stack during parsing.
	CallNode(std::stack<IBuildItem*>* buildStack);

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_CALL_NODE;}
	//int getLineNum(void);
	//std::string getValue(void);

	// Required form of toString().
	std::string toString(void);

	// Required implementation to conduct semantic analysis on the AST.
	void checkSemantics(SymbolTable* symTable, std::vector<Symbol*>* symList);

	// Required implementation to conduct type checking on the AST.
	VarType_e checkTyping(void);

	// Required implementation to conduct optimizations of the AST.
	int optimizeAST(std::unordered_map<Symbol*,int>* constList);

	// Required implementation to conduct translation of the AST.
	genValue_t translate(AsmMaker* asmGen);

	// Required implementation to compute an ExpNode's specific constant.
	void computeConst(void) {/* No actions */}

	// Required implementation to translate an ExpNode's specific kind.
	void genExp(AsmMaker*, RegName_e lhs, RegName_e rhs) {return;}

	// Destructor for deleting children.
	~CallNode(void);

private:
	// ID information for function being called.
	IDNode* m_id;

	// Ordered list of arguments/expressions.
	std::vector<IExpNode*> m_argList;

	// Linked symbol with function's larger information.
	Symbol* m_sym;
};

///////////////////////////
// === Logical Nodes === //
///////////////////////////

// Node representing an AND (&) operation.
class AndNode: public IExpNode {
public:
	// Constructor- builds node from build stack during parsing.
	AndNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_AND_NODE;}

	// Required form of toString().
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}

	// Required implementation to compute an ExpNode's specific constant.
	void computeConst(void);

	// Required implementation to translate an ExpNode's specific kind.
	void genExp(AsmMaker*, RegName_e lhs, RegName_e rhs);
};

// Node representing an OR (|) operation.
class OrNode: public IExpNode {
public:
	// Constructor- builds node from build stack during parsing.
	OrNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_OR_NODE;}

	// Required form of toString().
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}

	// Required implementation to compute an ExpNode's specific constant.
	void computeConst(void);

	// Required implementation to translate an ExpNode's specific kind.
	void genExp(AsmMaker*, RegName_e lhs, RegName_e rhs);
};

// Node representing a XOR (^) operation.
class XorNode: public IExpNode {
public:
	// Constructor- builds node from build stack during parsing.
	XorNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_XOR_NODE;}

	// Required form of toString().
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}

	// Required implementation to compute an ExpNode's specific constant.
	void computeConst(void);

	// Required implementation to translate an ExpNode's specific kind.
	void genExp(AsmMaker*, RegName_e lhs, RegName_e rhs);
};

// Node representing a logical/bitwise NOT (~) operation.
class LNotNode: public IExpNode {
public:
	// Constructor- builds node from build stack during parsing.
	LNotNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_LNOT_NODE;}

	// Required form of toString().
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}

	// Required implementation to compute an ExpNode's specific constant.
	void computeConst(void);

	// Required implementation to translate an ExpNode's specific kind.
	void genExp(AsmMaker*, RegName_e lhs, RegName_e rhs);
};

// Node representing a right shift (>>) operation.
class RShiftNode: public IExpNode {
public:
	// Constructor- builds node from build stack during parsing.
	RShiftNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_RSHIFT_NODE;}

	// Required form of toString().
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}

	// Required implementation to conduct type checking on the AST.
	VarType_e checkTyping(void);

	// Required implementation to compute an ExpNode's specific constant.
	void computeConst(void);

	// Required implementation to translate an ExpNode's specific kind.
	void genExp(AsmMaker*, RegName_e lhs, RegName_e rhs);
};

// Node representing a left shift (<<) operation.
class LShiftNode: public IExpNode {
public:
	// Constructor- builds node from build stack during parsing.
	LShiftNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_LSHIFT_NODE;}

	// Required form of toString().
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}

	// Required implementation to compute an ExpNode's specific constant.
	void computeConst(void);

	// Required implementation to translate an ExpNode's specific kind.
	void genExp(AsmMaker*, RegName_e lhs, RegName_e rhs);
};

//////////////////////////////
// === Arithmetic Nodes === //
//////////////////////////////

// Node representing an addition (+) operation.
class PlusNode: public IExpNode {
public:
	// Constructor- builds node from build stack during parsing.
	PlusNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_PLUS_NODE;}

	// Required form of toString().
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}

	// Required implementation to compute an ExpNode's specific constant.
	void computeConst(void);

	// Required implementation to translate an ExpNode's specific kind.
	void genExp(AsmMaker*, RegName_e lhs, RegName_e rhs);
};

// Node representing a subtraction or arithmetic negation (-) operation.
class MinusNode: public IExpNode {
public:
	// Constructor- builds node from build stack during parsing.
	MinusNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_MINUS_NODE;}

	// Required form of toString().
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}

	// Required implementation to conduct type checking on the AST.
	VarType_e checkTyping(void);

	// Required implementation to compute an ExpNode's specific constant.
	void computeConst(void);

	// Required implementation to translate an ExpNode's specific kind.
	void genExp(AsmMaker*, RegName_e lhs, RegName_e rhs);
};

///////////////////////////
// === Boolean Nodes === //
///////////////////////////

// Node representing a greater than (>) operation.
class GrtNode: public IExpNode {
public:
	// Constructor- builds node from build stack during parsing.
	GrtNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_GRT_NODE;}

	// Required form of toString().
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}

	// Required implementation to compute an ExpNode's specific constant.
	void computeConst(void);

	// Required implementation to translate an ExpNode's specific kind.
	void genExp(AsmMaker*, RegName_e lhs, RegName_e rhs);
};

// Node representing a less than (<) operation.
class LtNode: public IExpNode {
public:
	// Constructor- builds node from build stack during parsing.
	LtNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_LT_NODE;}

	// Required form of toString().
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}

	// Required implementation to compute an ExpNode's specific constant.
	void computeConst(void);

	// Required implementation to translate an ExpNode's specific kind.
	void genExp(AsmMaker*, RegName_e lhs, RegName_e rhs);
};

// Node representing a greater than or equal to (>=) operation.
class GeqNode: public IExpNode {
public:
	// Constructor- builds node from build stack during parsing.
	GeqNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_GEQ_NODE;}

	// Required form of toString().
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}

	// Required implementation to compute an ExpNode's specific constant.
	void computeConst(void);

	// Required implementation to translate an ExpNode's specific kind.
	void genExp(AsmMaker*, RegName_e lhs, RegName_e rhs);
};

// Node representing a less than or equal to (<=) operation.
class LeqNode: public IExpNode {
public:
	// Constructor- builds node from build stack during parsing.
	LeqNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_LEQ_NODE;}

	// Required form of toString().
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}

	// Required implementation to compute an ExpNode's specific constant.
	void computeConst(void);

	// Required implementation to translate an ExpNode's specific kind.
	void genExp(AsmMaker*, RegName_e lhs, RegName_e rhs);
};

// Node representing an equals (==) operation.
class EqNode: public IExpNode {
public:
	// Constructor- builds node from build stack during parsing.
	EqNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_EQ_NODE;}

	// Required form of toString().
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}

	// Required implementation to compute an ExpNode's specific constant.
	void computeConst(void);

	// Required implementation to translate an ExpNode's specific kind.
	void genExp(AsmMaker*, RegName_e lhs, RegName_e rhs);
};

// Node representing a not equals (!=) operation.
class NeqNode: public IExpNode {
public:
	// Constructor- builds node from build stack during parsing.
	NeqNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_NEQ_NODE;}

	// Required form of toString().
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}

	// Required implementation to compute an ExpNode's specific constant.
	void computeConst(void);

	// Required implementation to translate an ExpNode's specific kind.
	void genExp(AsmMaker*, RegName_e lhs, RegName_e rhs);
};

// Node representing a boolean not (!) operation.
class BNotNode: public IExpNode {
public:
	// Constructor- builds node from build stack during parsing.
	BNotNode(std::stack<IBuildItem*>* buildStack) {
		m_lineNum = this->initFromStack(buildStack);
	}

	// Required functions to build nodes while parsing.
	uint8_t getBuildType(void) {return PARSE_BNOT_NODE;}

	// Required form of toString().
	std::string toString(void) {
		return this->toExpString(getBuildType());
	}

	// Required implementation to compute an ExpNode's specific constant.
	void computeConst(void);

	// Required implementation to translate an ExpNode's specific kind.
	void genExp(AsmMaker*, RegName_e lhs, RegName_e rhs);
};

#endif /* NODECONCRETES_H_ */
