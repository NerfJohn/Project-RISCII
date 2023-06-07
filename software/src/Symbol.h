/*
 * Symbol.h
 */

#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <string>
#include <vector>
#include "VarType.h"

// TODO
class Symbol {
public:
	// TODO
	std::string m_id;
	int m_declLineNum = -1;
	VarType_e m_type;
	bool m_isFunc;
	std::vector<VarType_e> m_argList;
	bool m_isGlobal;

	// TODO
	bool m_isInit = false;
	bool m_isUsed = false;

	// TODO
	std::string toName(void){return "{\"" + m_id + "\" at " +
			std::to_string(m_declLineNum) + "}";}
};

#endif /* SYMBOL_H_ */
