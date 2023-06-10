/*
 * NodeAbstracts.h
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

#ifndef NODEABSTRACTS_H_
#define NODEABSTRACTS_H_

#include <stack>
#include "IBuildItem.h"
#include "ParseTable.h"
#include "SymbolTable.h"
#include "VarType.h"

// TODO
class IDeclNode;

// TODO
enum OptResult_e {
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

// TODO
class IASTNode: public IBuildItem {
public:
	// TODO
	static IDeclNode* m_curFunc;

	// TODO
	static int m_numDeletes;

	// TODO
	virtual uint8_t getBuildType(void) 	= 0;				// Node defined
	virtual int getLineNum(void)		{return m_lineNum;}
	virtual std::string getValue(void)	{return "";}		// "no value"

	// TODO
	static std::string nodeToString(ParseActions node);

	// TODO
	virtual std::string toString(void) = 0;

	// TODO
	virtual void checkSemantics(SymbolTable* symTable,
								std::vector<Symbol*>* symList) = 0;

	// TODO
	virtual VarType_e checkTyping(void) = 0;

	// TODO
	virtual int optimizeAST(std::unordered_map<Symbol*,int>* constList) = 0;

	// TODO
	static VarType_e getNewTyping(VarType_e lhs, VarType_e rhs, int lineNum);
	static bool canAssignTyping(VarType_e goal, VarType_e start);

	// TODO
	virtual ~IASTNode(void) {/* count optimized nodes */ m_numDeletes++;}

protected:
	// TODO
	int m_lineNum = -1;	// Default = "no line number" for MsgLog
};

///////////////////////
// === IDeclNode === //
///////////////////////

// TODO
class IDeclNode: public virtual IASTNode {
public:
	// TODO
	VarType_e getType(void) {return m_varType;}

	// TODO
	void setReturn(bool hasRet) {m_hasRet = hasRet;}

	// TODO
	virtual ~IDeclNode(void) {/* Symbol owner */ delete m_sym;}

protected:
	// TODO
	VarType_e m_varType;
	std::string m_id;

	// TODO
	Symbol* m_sym;

	// TODO
	bool m_hasRet = false;
};

///////////////////////
// === IStmtNode === //
///////////////////////

// TODO
class IStmtNode: public virtual IASTNode {};

//////////////////////
// === IExpNode === //
//////////////////////

// TODO
class IExpNode: public virtual IASTNode {
public:
	// TODO
	int initFromStack(std::stack<IBuildItem*>* buildStack);
	std::string toExpString(std::uint8_t type);

	// TODO
	bool isUnary(void) {return (m_lhs == nullptr) && (m_rhs != nullptr);}
	bool isUnit(void) {return (m_lhs == nullptr) && (m_rhs == nullptr);}

	// TODO
	virtual void checkSemantics(SymbolTable* symTable,
								std::vector<Symbol*>* symList);

	// TODO
	VarType_e checkTyping(void);

	// TODO
	virtual int optimizeAST(std::unordered_map<Symbol*,int>* constList);

	// TODO
	bool isConstVal(void) {return m_constVal <= OPT_CONST_MAX;}
	int getConstVal(void) {return m_constVal;}
	virtual void computeConst(void) = 0;

	// TODO
	VarType_e getType(void) {return m_varType;}

	// TODO
	virtual ~IExpNode(void);

protected:
	// TODO
	IExpNode* m_lhs = nullptr;
	IExpNode* m_rhs = nullptr;

	// TODO
	VarType_e m_varType;

	// TODO
	int m_constVal = OPT_VAL_UNKNOWN;
};

#endif /* NODEABSTRACTS_H_ */
