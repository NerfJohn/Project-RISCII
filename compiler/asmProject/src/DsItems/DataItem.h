/*
 * DataItem.h
 *
 *  Created on: Apr 2, 2024
 *      Author: John
 */

#ifndef SRC_DSITEMS_DATAITEM_H_
#define SRC_DSITEMS_DATAITEM_H_

#include <stack>
#include <deque>
#include "ABuildItem.h"
#include "../Parsing/AsmToken.h"
#include "SectionType_e.h"

// TODO- item representing data allocation line item.
class DataItem: public ABuildItem {
public:
	// TODO- instances created from tokens on action stack.
	DataItem(std::stack<AsmToken*> actStack);

	// TODO- analyze item for later checks and translation.
	void doAnalysis(AnalysisData_t* model);

	// TODO- type check item- return indicates "hasError".
	bool doChecking(AnalysisData_t* model);

	// TODO- resolve last of (meta) data before translating.
	void resolveData(TranslationData_t* model);

	// TODO- get section item is in (most do, some don't).
	SectionType_e getSection(void);

	// TODO- "to string" for ease of debugging.
	std::string asStr(void);

	// Std. destructor.
	~DataItem(void);

private:
	// Typical Data information.
	SyntaxToken_e m_type;        // data type (limited to data tokens)
	std::deque<uint8_t> m_bytes; // init value (if provided), little endian
	SectionType_e m_section;     // section data is in

	// TODO- getter/setter for handling ints to/from byte array.
	int bytesAsInt(void);
	void setIntInBytes(int value);
};


#endif /* SRC_DSITEMS_DATAITEM_H_ */
