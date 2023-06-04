/*
 * Symbol.h
 */

#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <string>
#include "VarType.h"

// TODO
class Symbol {
public:
	// TODO
	std::string m_id;
	int m_declLineNum = -1;
	VarType_e m_type;
	int m_numArgs;

	// TODO
	bool m_isInit = false;
	bool m_isUsed = false;

	// TODO
	bool isFunc(void) {return m_numArgs >= 0;}

	// TODO
	std::string toName(void){return "{" + m_id + " at " +
			std::to_string(m_declLineNum) + "}";}
};

#endif /* SYMBOL_H_ */
