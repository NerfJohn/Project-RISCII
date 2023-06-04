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

//////////////////////
// === IASTNode === //
//////////////////////

// TODO
class IASTNode: public IBuildItem {
public:
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
	virtual ~IASTNode(void) {/* Empty dtor */}

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
	//TODO
	IASTNode* asASTNode(void) {return (IASTNode*)(this);}

protected:
	// TODO
	VarType_e m_varType;
	std::string m_id;
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

protected:
	// TODO
	IExpNode* m_lhs = nullptr;
	IExpNode* m_rhs = nullptr;
};

#endif /* NODEABSTRACTS_H_ */
