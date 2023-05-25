/*
 * ScanToken.cpp
 */

#include "MsgLog.h"

#include "ScanToken.h"

using namespace std;

/*
 * Note:
 * The "conversion function" approach is very vulnerable to changes to
 * ScanTable.h's enumerator. Asserts provide some support in quick detection of
 * issues, but keep this in mind for future improvement/retrospective.
 */

// Helper function for string-ifying the type.
std::string ScanToken::typeToString(void) {
	// Return string-ified type.
	switch (m_type) {
		case SCAN_AND: return "AND";
		case SCAN_OR: return "OR";
		case SCAN_XOR: return "XOR";
		case SCAN_LNOT: return "LNOT";
		case SCAN_PLUS: return "PLUS";
		case SCAN_MINUS: return "MINUS";
		case SCAN_LCURLY: return "LCURLY";
		case SCAN_RCURLY: return "RCURLY";
		case SCAN_LPAREN: return "LPAREN";
		case SCAN_RPAREN: return "RPAREN";
		case SCAN_SEMICOLON: return "SEMICOLON";
		case SCAN_COMMA: return "COMMA";
		case SCAN_RSHIFT: return "RSHIFT";
		case SCAN_GEQ: return "GEQ";
		case SCAN_LSHIFT: return "LSHIFT";
		case SCAN_LEQ: return "LEQ";
		case SCAN_EQ: return "EQ";
		case SCAN_NEQ: return "NEQ";
		case SCAN_CLITERAL: return "C-LIT";
		case SCAN_COMMENT: return "COMMENT";
		case SCAN_GRT: return "GRT";
		case SCAN_LT: return "LT";
		case SCAN_ASSIGN: return "ASSIGN";
		case SCAN_BNOT: return "LNOT";
		case SCAN_ID: return "ID";
		case SCAN_ILITERAL: return "I-LIT";
		case SCAN_HLITERAL: return "H-LIT";
		case SCAN_IF: return "IF";
		case SCAN_INT: return "INT";
		case SCAN_CHAR: return "CHAR";
		case SCAN_WHILE: return "WHILE";
		case SCAN_RETURN: return "RETURN";
		case SCAN_UNSIGNED: return "UNSIGNED";
		default:
			// Implies type is corrupted- assert source code is faulty.
			string msg("ScanToken::typeToString() called when m_type = ");
			MsgLog::logASSERT( msg + to_string((int)(m_type)));
	}

	// (Never reached, but satisfy compiler warning.)
	return "";
}

// "Initialize" constructor for ScanToken object.
ScanToken::ScanToken(const ScanTableStates type,
			  	  	 const int lineNum,
					 const string value) {
	// Ensure type is a concrete token.
	if (type < SCAN_END_MIN ||
		type > SCAN_END_MAX ||
		type == SCAN_ERROR) {
		// Implies caller code is wrong- assert source code is faulty.
		string msg("ScanToken constructor called with type = ");
		MsgLog::logASSERT( msg + to_string((int)(type)));
	}

	// Determine if value needs to be populated.
	bool needsValue = type == SCAN_ID ||
					  type == SCAN_CLITERAL ||
					  type == SCAN_HLITERAL ||
					  type == SCAN_ILITERAL ||
					  type == SCAN_COMMENT;

	/*
	 * Note:
	 * This is a pretty janky way of saving extra data for IDs/literals/etc.
	 * It'll work for now, but this may a good place to look for future
	 * improvement/retrospective in micro-architecture design.
	 */

	// Save the given values.
	m_type = type;
	m_lineNum = lineNum;
	if (needsValue) m_value = value;
}

// Creates string version of ScanToken object.
string ScanToken::toString(void) {
	// Prepare string (with punctuation).
	string tknStr = "{";

	// Add type using conversion function.
	tknStr += this->typeToString();

	// Add line number.
	tknStr += " at " + to_string(m_lineNum);

	// Add value/data if applicable.
	if (m_value.size()) {tknStr += " = \"" + m_value + "\"";}

	// Finish punctuation and return.
	return tknStr + "}";
}
