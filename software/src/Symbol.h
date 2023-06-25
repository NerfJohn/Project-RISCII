/*
 * Symbol.h
 */

#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <string>
#include <vector>
#include "VarType.h"

// Class to hold pertinent data shared between declarations and uses.
class Symbol {
public:
	// Generic information.
	std::string m_id;
	int m_declLineNum = -1;
	VarType_e m_type;

	// Additional arg info (for function defs/calls).
	std::vector<VarType_e> m_argList;

	// Additional value info for translation/computing.
	int m_memValue;
	int m_constVal;

	// Various flags.
	bool m_isInit = false;
	bool m_isUsed = false;
	bool m_isGlobal;
	bool m_isFunc;

	// String-ifier for symbol- most data withheld for brevity.
	std::string toName(void){return "{\"" + m_id + "\" at " +
			std::to_string(m_declLineNum) + "}";}
};

#endif /* SYMBOL_H_ */
