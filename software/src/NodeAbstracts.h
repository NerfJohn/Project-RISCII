/*
 * NodeAbstracts.h
 */

#ifndef NODEABSTRACTS_H_
#define NODEABSTRACTS_H_

#include <stack>
#include "AsmMaker.h"
#include "IBuildItem.h"
#include "MemHandler.h"
#include "ParseTable.h"
#include "SymbolTable.h"
#include "VarType.h"

// Forward declare to allow for static access to function nodes.
class IDeclNode;

// Enum for representing the node's "constant value" status.
enum OptResult_e {
	/* -32768 - 65535 reserved for actual constant value */
	OPT_CONST_MAX 		= 0x0ffff,
	OPT_VAL_UNKNOWN 	= 0x10000,
	OPT_KEEP			= 0x10001,
	OPT_DEL_THIS		= 0x10002,
	OPT_BURN_INCLUDE	= 0x10003,
	OPT_BURN_EXCLUDE	= 0x10004
};

//////////////////////
// === IASTNode === //
//////////////////////

// Abstract class to represent all nodes.
class IASTNode: public IBuildItem {
public:
	// Access point to current function being processed.
	static IDeclNode* m_curFunc;

	// Number of nodes delted/pruned from the tree.
	static int m_numDeletes;

	// Next available address in the .data section of RAM.
	static int m_nextDataAddr;

	// True if accumulator currently holds an intermediate value, else false.
	static bool s_accumIsInter;

	// Required functions to build nodes while parsing.
	virtual uint8_t getBuildType(void) 	= 0;				// Node defined
	virtual int getLineNum(void)		{return m_lineNum;}
	virtual std::string getValue(void)	{return "";}		// "no value"

	// toString() function for node names (as provided by ParseActions).
	static std::string nodeToString(ParseActions node);

	// Every node is required to have some form of toString().
	virtual std::string toString(void) = 0;

	// Required implementation to conduct semantic analysis on the AST.
	virtual void checkSemantics(SymbolTable* symTable,
								std::vector<Symbol*>* symList) = 0;

	// Required implementation to conduct type checking on the AST.
	virtual VarType_e checkTyping(void) = 0;

	// Required implementation to conduct optimizations of the AST.
	virtual int optimizeAST(std::unordered_map<Symbol*,int>* constList) = 0;

	// Required implementation to conduct translation of the AST.
	virtual genValue_t translate(AsmMaker* asmGen) = 0;

	// Static helpers to encapsulate typing rules.
	static VarType_e getNewTyping(VarType_e lhs, VarType_e rhs, int lineNum);
	static bool canAssignTyping(VarType_e goal, VarType_e start);

	// Common destructor to keep count of deleted nodes.
	virtual ~IASTNode(void) {/* count optimized nodes */ m_numDeletes++;}

protected:
	// Line number of tokens/nodes used to create the node.
	int m_lineNum = -1;	// Default = "no line number" for MsgLog
};

///////////////////////
// === IDeclNode === //
///////////////////////

// Abstract class to represent nodes that declare symbol-owning nodes.
class IDeclNode: public virtual IASTNode {
public:
	// Required functions to build nodes while parsing.
	VarType_e getType(void) {return m_varType;}

	// Helper function for functions figuring out "no return" warning.
	void setReturn(bool hasRet) {m_hasRet = hasRet;}

	// Destructor- delete linked symbol as its official owner.
	virtual ~IDeclNode(void) {/* Symbol owner */ delete m_sym;}

protected:
	// Common variables per declarations.
	VarType_e m_varType;
	std::string m_id;

	// Symbol with decl information- declarations considered owner.
	Symbol* m_sym;

	// Helper flag for functions figuring out "no return" warning.
	bool m_hasRet = false;
};

///////////////////////
// === IStmtNode === //
///////////////////////

// Abstract class for nodes representing line statements.
class IStmtNode: public virtual IASTNode {};

//////////////////////
// === IExpNode === //
//////////////////////

// Abstract class for nodes representing expression operators/operands.
class IExpNode: public virtual IASTNode {
public:
	// Common function for initializing an ExpNode during parsing.
	int initFromStack(std::stack<IBuildItem*>* buildStack);

	// Common function for string-ifying an ExpNode.
	std::string toExpString(std::uint8_t type);

	// Common function for semantically analyzing an ExpNode.
	virtual void checkSemantics(SymbolTable* symTable,
								std::vector<Symbol*>* symList);

	// Common function for type checking an ExpNode.
	VarType_e checkTyping(void);

	// Common function for optimizing an ExpNode.
	virtual int optimizeAST(std::unordered_map<Symbol*,int>* constList);

	// Common function for translating an ExpNode.
	virtual genValue_t translate(AsmMaker* asmGen);

	// Required implementation to compute an ExpNode's specific constant.
	virtual void computeConst(void) = 0;

	// Required implementation to translate an ExpNode's specific kind.
	virtual void genExp(AsmMaker*, RegName_e lhs, RegName_e rhs) = 0;

	// Helper functions to determine nature of the ExpNode's operands.
	bool isUnary(void) {return (m_lhs == nullptr) && (m_rhs != nullptr);}
	bool isUnit(void) {return (m_lhs == nullptr) && (m_rhs == nullptr);}

	// Helper functions to determine nature of the ExpNode's value.
	bool isConstVal(void) {return m_constVal <= OPT_CONST_MAX;}
	int getConstVal(void) {return m_constVal;}

	// Getter for the ExpNode's overall typing.
	VarType_e getType(void) {return m_varType;}

	// Common destructor for ExpNodes.
	virtual ~IExpNode(void);

protected:
	// Links to ExpNode's children- null by default.
	IExpNode* m_lhs = nullptr;
	IExpNode* m_rhs = nullptr;

	// Overall typing of the ExpNode.
	VarType_e m_varType;

	// Overall value of the ExpNode.
	int m_constVal = OPT_VAL_UNKNOWN;
};

#endif /* NODEABSTRACTS_H_ */
