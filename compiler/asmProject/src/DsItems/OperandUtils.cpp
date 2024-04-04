/*
 * OperandUtils.cpp
 *
 *  Created on: Apr 1, 2024
 *      Author: John
 */

#include "OperandUtils.h"

using namespace std;

// TODO- Converts immediate string (decimal or hex) to int for ease.
int OperandUtils_immStrToInt(std::string immStr) {
	/* Note- assumes string is properly formed */

	// Determine base of immediate.
	bool isHex = (immStr.size() >= 3) &&	// is long enough to be hex...
				 (immStr[0] == '0') &&      // and begins with hex prefix...
				 (immStr[1] == 'x');

	// Convert to integer with correct based.
	int retInt = 0;
	if (isHex) {
		string rawHex = immStr.substr(2,immStr.size() - 2); // chop off "0x"
		retInt = strtol(rawHex.c_str(), NULL, 16);
	}
	else {
		retInt = strtol(immStr.c_str(), NULL, 10);
	}

	// Return converted int.
	return retInt;
}

// TODO- Converts register string to int for ease.
int OperandUtils_regStrToInt(std::string regStr) {
	/* Note- assumes string is properly formed */

	// Register value is 2nd char (0-7, ie mono-char).
	return (int)(regStr[1]) - 0x30;
}

// TODO- Converts string literal to vector of bytes.
std::vector<uint8_t> OperandUtils_strLitToBytes(std::string strLit) {
	/* Note- assumes string is properly formed */

	// Convert each char (nix the end quotations).
	vector<uint8_t> retBytes;
	for (int i = 1; i < (int)(strLit.size() - 1); i++) {
		// Most (non-escape) chars can just be inserted directly...
		if (strLit[i] != '\\') {
			retBytes.push_back(strLit[i]);
		}
		else { // ...but escape chars require more handling
			// Factor for escape char being processed now.
			i++;

			// Determine byte value.
			uint8_t escByte = 0;
			switch (strLit[i]) {
				case 'n': escByte = '\n'; break;
				case 'r': escByte = '\r'; break;
				case 't': escByte = '\t'; break;
				case '0': escByte = '\0'; break;
				case '\\': escByte = '\\'; break;
				case 'x':
					// 2-digit hex code follows 'x'.
					escByte = (strLit[i+1] - 0x30) << 4;
					escByte += (strLit[i+2] - 0x30);

					// Adjust for two extra chars being processed.
					i += 2;

					break;
			}

			// Push escaped byte to vector.
			retBytes.push_back(escByte);
		}
	}

	// Return converted vector of bytes.
	return retBytes;
}

// TODO- Converts section to enum for ease.
SectionType_e OperandUtils_sectStrToEnum(std::string sectStr) {
	/* Note- assumes string is properly formed */

	// Register value is 2nd char (specific chars).
	SectionType_e retVal = SECTION_INVALID;
	switch (sectStr[1]) {
		case 'r': retVal = SECTION_RODATA; break;
		case 'd': retVal = SECTION_DATA; break;
		case 'b': retVal = SECTION_BSS; break;
	}

	// Return converted string.
	return retVal;
}
