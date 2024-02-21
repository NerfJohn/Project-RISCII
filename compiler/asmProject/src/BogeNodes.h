/*
 * BogeNodes.h
 *
 *  Created on: Feb 19, 2024
 *      Author: John
 */

#ifndef SRC_BOGENODES_H_
#define SRC_BOGENODES_H_

#include <stack>
#include <deque>
#include "AsmToken.h"

// TODO- base build item
class IBuildItem {
public:
	// TODO
	virtual void print(void) = 0;

	virtual ~IBuildItem(void) {}
};

// TODO- desc.
class InstrItem: public IBuildItem {
public:
	// TODO
	InstrItem(std::stack<AsmToken>* actStack);

	// TODO
	void print(void);

private:
	// TODO
	std::string m_immStr;
	std::deque<std::string> m_regsStr;
	std::string m_flagStr;
	std::string m_opStr;
	SyntaxToken_e m_opTkn;
};

// TODO- desc.
class FuncItem: public IBuildItem {
public:
	// TODO
	FuncItem(std::stack<AsmToken>* actStack);

	// TODO
	void print(void);

private:
	// TODO
	std::string m_labelStr;
	std::deque<std::string> m_regsStr;
	std::string m_funcStr;
	SyntaxToken_e m_funcTkn;

};

// TODO- desc.
class DataItem: public IBuildItem {
public:
	// TODO
	DataItem(std::stack<AsmToken>* actStack);

	// TODO
	void print(void);

private:
	// TODO
	std::string m_immStr;
	std::string m_str;
	std::string m_dataStr;
	SyntaxToken_e m_dataTkn;

};

// TODO- desc.
class DirectiveItem: public IBuildItem {
public:
	// TODO
	DirectiveItem(std::stack<AsmToken>* actStack);

	// TODO
	void print(void);

private:
	// TODO
	std::string m_labelStr;
	std::string m_directStr;
	SyntaxToken_e m_directTkn;
};

// TODO- desc.
class LabelItem: public IBuildItem {
public:
	// TODO
	LabelItem(std::stack<AsmToken>* actStack);

	// TODO
	void print(void);

private:
	// TODO
	std::string m_labelStr;
};


#endif /* SRC_BOGENODES_H_ */
