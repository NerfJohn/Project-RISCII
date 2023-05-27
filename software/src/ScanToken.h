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
 * of a scanned token. The object acts as an immutable object that can be
 * read and string-ified as needed for logic and reporting.
 */
class ScanToken {
public:
	/*
	 * "Initialize" constructor for ScanToken object.
	 *
	 * Constructor takes in type, lineNum, and value to create a typical token.
	 * Type reflects any "concrete" scan table end state (ie NOT a SCAN_ERROR)
	 * while value is populated for identifiers, literals, and comments ONLY.
	 */
	ScanToken(const ScanTableStates type,
			  const int lineNum,
			  const std::string value = "");

	/*
	 * Typical getters for ScanToken's data.
	 */
	ScanTableStates getType(void) 		{return m_type;}
	int getLineNum(void) 				{return m_lineNum;}
	std::string getValue(void)			{return m_value;}

	/*
	 * Static public function for string-ifying a scan table state.
	 *
	 * Simple way to string-ify a scan table end state for various uses (eg
	 * logging verbose info or errors). Note that this function is intended
	 * to string-ify "concrete" tokens ONLY.
	 *
	 * @return string version of given type
	 */
	static std::string typeToString(ScanTableStates type);

	/*
	 * Creates string version of ScanToken object.
	 *
	 * String specifies object's type, line number, and, if applicable, value.
	 * For readability, type is converted into a readable ASCII string.
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
};

#endif /* SCANTOKEN_H_ */
