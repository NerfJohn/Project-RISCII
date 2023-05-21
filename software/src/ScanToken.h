/*
 * ScanToken.h
 */

#include <string>
#include "ScanTable.h"

#ifndef SCANTOKEN_H_
#define SCANTOKEN_H_

/*
 * Class representing a token created by using the scan table.
 *
 * ScanToken contains basic information about the type, location, and value
 * of a scanned token. The object acts as an immutable token that can be
 * read and string-ified as needed for logic and reporting.
 */
class ScanToken {
public:
	/*
	 * "Initialize" constructor for ScanToken object.
	 *
	 * Constructor takes in type, lineNum, and value to create a typical token.
	 * Type reflect any scan table end state (SCAN_ERROR not recommended) while
	 * value is populated for identifiers and literals ONLY.
	 */
	ScanToken(const ScanTableStates type,
			  const int lineNum,
			  const std::string value);

	/*
	 * Typical getters for ScanToken's data.
	 */
	ScanTableStates getType(void) 		{return m_type;}
	int getLineNum(void) 				{return m_lineNum;}
	std::string getValue(void)			{return m_value;}

	/*
	 * Creates string version of ScanToken object.
	 *
	 * String specifies object's type, line number, and, if applicable, value.
	 * For readability, type is converted into a readable ASCII char/string
	 * using a privately saved table.
	 *
	 * @return string version of the ScanToken object
	 */
	std::string toString(void);

	/*
	 * Std. Destructor.
	 */
	virtual ~ScanToken() {/* Empty dtor */}

private:
	// Key token data to track.
	ScanTableStates m_type;	// Type of token being represented
	int m_lineNum;			// Line number of token for warnings/errors
	std::string m_value;	// Extra data for identifiers/literals

	// Static table to help with converting object into a string.
	static const char* s_STR_TABLE[];
};

#endif /* SCANTOKEN_H_ */
