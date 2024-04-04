/*
 * DataItem.cpp
 *
 *  Created on: Apr 2, 2024
 *      Author: John
 */

#include "OperandUtils.h"

#include "DataItem.h"

using namespace std;

// TODO- getter/setter for handling words to/from byte array.
int DataItem::bytesAsInt(void) {
	// Ensure at least 4 bytes exist- fail to 0 (TODO- be better!).
	if (m_bytes.size() < 4) {return 0;}

	// Extract first 2 bytes- interpret as unified int.
	return (int)((m_bytes[3] << 24) |
				 (m_bytes[2] << 16) |
				 (m_bytes[1] << 8)  |
				 (m_bytes[0]));
}
void DataItem::setIntInBytes(int value) {
	// Ensure two bytes exist in local array.
	while(m_bytes.size() < 4) {m_bytes.push_back(0);}

	// Implant int's 4 bytes- little endian style.
	m_bytes[0] = (uint8_t)(value & 0xff);
	m_bytes[1] = (uint8_t)((value >> 8) & 0xff);
	m_bytes[2] = (uint8_t)((value >> 16) & 0xff);
	m_bytes[3] = (uint8_t)((value >> 24) & 0xff);
}

// TODO- instances created from tokens on action stack.
DataItem::DataItem(std::stack<AsmToken*> actStack) {
	// Initial values.
	m_type = TOKEN_INVALID;
	m_bytes = {};
	m_section = SECTION_INVALID;

	/*
	 * TODO- For now expect these:
	 * .word <value> <section>
	 * .arry <strs...> <section>
	 */

	// Process section.
	m_section = OperandUtils_sectStrToEnum(actStack.top()->getValue());
	actStack.pop();

	// Process value (as present).
	if (actStack.top()->getType() == TOKEN_IMMEDIATE) {
		int value = OperandUtils_immStrToInt(actStack.top()->getValue());
		this->setIntInBytes(value);
		actStack.pop();
	}

	// Process string literals (as present).
	while(actStack.top()->getType() == TOKEN_STRING) {
		// Parse string into chars (ie parse escape chars).
		vector<uint8_t> parseBytes = OperandUtils_strLitToBytes(actStack.top()->getValue());

		// Add chars (in reverse- undoes stack's push/pop ordering).
		for (int i = parseBytes.size() - 1; 0 <= i; i--) {
			m_bytes.push_front(parseBytes[i]);
		}

		// Parsed- pop token. // TODO- memory leak w/ pointers. To fix in many places..!
		actStack.pop();
	}

	// Record type (owner of origin).
	m_type = actStack.top()->getType();
	m_origin = actStack.top()->getOrigin();
	actStack.pop();
}

// TODO- "to string" for ease of debugging.
std::string DataItem::asStr(void) {
	// Get prefix.
	string retStr = "{DataItem: ";

	// Get contents of instance.
	retStr += "m_type(#) = " + to_string(m_type) + ", ";
	retStr += "m_bytes = {";
	for (int i = 0; i < (int)m_bytes.size(); i++) {
		retStr += to_string(m_bytes[i]) + ", ";
	}
	retStr += "}, ";
	retStr += "m_origin = \"" + m_origin + "\"";

	// Get suffix.
	retStr += "}";

	// Return string-ified AsmToken.
	return retStr;
}

// Std. destructor.
DataItem::~DataItem(void) {/* no non-std pointers */}
