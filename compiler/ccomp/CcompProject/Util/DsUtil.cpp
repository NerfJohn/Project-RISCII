/*
 * DsUtil.cpp: TODO
 */

#include "Util/DsUtil.h"

using namespace std;

//==============================================================================

// Definitions for root sizes of items.
#define CHAR_BYTES 1
#define INT_BYTES  2
#define PTR_BYTES  2

//==============================================================================
// TODO
std::size_t DsUtil_sizeof(Type_t const& type) {
	// Size to return.
	size_t retSize = 0;

	// Determine the root size.
	if (type.m_ptrCnt > 0) {
		retSize = PTR_BYTES;
	}
	else if (type.m_type == PRIMITIVE_CHAR) {
		retSize = CHAR_BYTES;
	}
	else if (type.m_type == PRIMITIVE_INT) {
		retSize = INT_BYTES;
	}

	// Return size.
	return retSize;
}
