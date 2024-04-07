/*
 * DataItem.cpp
 *
 *  Created on: Apr 2, 2024
 *      Author: John
 */

#include "OperandUtils.h"
#include "../Translate/TargetUtils.h"

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

	// Keep bytes aligned to hardware's memory access precision.
	int paddingNum = m_bytes.size() % TARGET_ACCESS_SIZE;
	while (paddingNum) {m_bytes.push_back(0); paddingNum--;}

	// Record type (owner of origin).
	m_type = actStack.top()->getType();
	m_origin = actStack.top()->getOrigin();
	actStack.pop();
}

// TODO- analyze item for later checks and translation.
void DataItem::doAnalysis(AnalysisData_t* model) {
	// Bookkeep.
	int bytesUsed = (m_type == TOKEN_WORD) ? 2 : (int)(m_bytes.size());
	if (m_section == SECTION_DATA) {model->m_dataLen += bytesUsed;}
	else {model->m_bssLen += bytesUsed;}
	model->m_lastAddrItem = this;
}

// TODO- type check item- return indicates "hasError".
bool DataItem::doChecking(AnalysisData_t* model) {
	// Return code.
	bool hasErr = false;

	// Re-size word value, reporting if truncation occurred.
	if (m_type == TOKEN_WORD) {
		// Convert and warn as needed.
		int oldVal = this->bytesAsInt();
		int newVal = TargetUtils_sizeToWord(oldVal);
		if (oldVal != newVal) {
			cout << "WARN (" << m_origin << "): Word truncation (";
			cout << "oldVal=" << oldVal << ", ";
			cout << "newVal=" << newVal << ")" << endl;

			hasErr = true;
		}

		// Save for translation.
		this->setIntInBytes(newVal);
	}

	// Return result of checking.
	return hasErr;
}

// TODO- resolve last of (meta) data before translating.
void DataItem::resolveData(TranslationData_t* model) {
	// TODO- could resolve value into parts here instead of type checking.

	// Determine size of data.
	int size = (m_type == TOKEN_WORD) ? 2 : (int)(m_bytes.size());

	// Record/adjust address based on given location.
	if (m_section == SECTION_DATA) {
		m_address = model->m_nextDataAddr;
		model->m_nextDataAddr += size;
	}
	else { // otherwise, must be in bss section.
		m_address = model->m_nextBssAddr;
		model->m_nextBssAddr += size;
	}
}

// TODO- get section item is in (most do, some don't).
SectionType_e DataItem::getSection(void) {
	// Data may vary on section- check it's typing.
	return m_section;
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
