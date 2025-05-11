/*
 * TypeUtil.cpp: Utilities related to managing type instances.
 */

#include "Domain/LexState_e.h"
#include "Util/AppUtil.h"

#include "Util/TypeUtil.h"

using namespace std;

//==============================================================================
// Initializes type using series of tokens.
void TypeUtil_initCore(Type_t& type, std::deque<Ptr<LexToken>>& tkns) {
	// Init.
	type.m_type   = CORE_TYPE_INVALID;
	type.m_ptrCnt = 0;
	type.m_hasLoc = true;

	// Parse.
	for (Ptr<LexToken> tkn : tkns) {
		switch(tkn->m_type) {
			case LEX_TKN_INT:  type.m_type = CORE_TYPE_INT;           break;
			case LEX_TKN_VOID: type.m_type = CORE_TYPE_VOID;          break;
			case LEX_TKN_STAR: type.m_ptrCnt++;                       break;
			default:           AppUtil_exitBug("initCore() bad tkn"); break;
		}
	}

	// Verify.
	if (CORE_TYPE_INVALID == type.m_type) {
		AppUtil_exitBug("initCore() no core");
	}
}

//==============================================================================
// TODO
bool TypeUtil_cmpType(Type_t const& typeA, Type_t const& typeB) {
	// Result.
	bool res = true;

	// Do comparisons.
	res = (typeA.m_type   == typeB.m_type)    &&
		  (typeA.m_ptrCnt == typeB.m_ptrCnt)  &&
		  (typeA.m_hasLoc == typeB.m_hasLoc);

	// Return.
	return res;
}
