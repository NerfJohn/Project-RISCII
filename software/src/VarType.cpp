/*
 * VarType.cpp
 */

#include "MsgLog.h"

#include "VarType.h"

using namespace std;

// Static function to string-ify a var type enum.
std::string VarType::toString(VarType_e type) {
	// Return string-ified variable type.
	switch (type) {
		case TYPE_INT: return "INT";
		case TYPE_CHAR: return "CHAR";
		case TYPE_UINT: return "UINT";
		case TYPE_UCHAR: return "UCHAR";
		case TYPE_LITERAL: return "LITERAL";
		default:
			// Implies type is corrupted- assert source code is faulty.
			string msg("VarType::toString() called when type = ");
			MsgLog::logASSERT(msg + to_string((int)(type)));
	}

	// (Never reached, but to appease the compiler...).
	return "";
}
