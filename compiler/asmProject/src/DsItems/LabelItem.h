/*
 * LabelItem.h
 *
 *  Created on: Apr 3, 2024
 *      Author: John
 */

#ifndef SRC_DSITEMS_LABELITEM_H_
#define SRC_DSITEMS_LABELITEM_H_

#include <stack>
#include "ABuildItem.h"
#include "../Parsing/AsmToken.h"

// TODO- represents declaration of label/special address.
class LabelItem: public ABuildItem {
public:
	// TODO- instances created from tokens on action stack.
	LabelItem(std::stack<AsmToken*> actStack);

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
	~LabelItem(void);

private:
	// Typical label information.
	std::string m_name; // name of label being declared
};

#endif /* SRC_DSITEMS_LABELITEM_H_ */
