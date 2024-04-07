/*
 * ControlItem.h
 *
 *  Created on: Apr 3, 2024
 *      Author: John
 */

#ifndef SRC_DSITEMS_CONTROLITEM_H_
#define SRC_DSITEMS_CONTROLITEM_H_

#include <stack>
#include "ABuildItem.h"
#include "../Parsing/AsmToken.h"

// TODO- represents control directive.
class ControlItem: public ABuildItem {
public:
	// TODO- instances created from tokens on action stack.
	ControlItem(std::stack<AsmToken*> actStack);

	// TODO- analyze item for later checks and translation.
	void doAnalysis(AnalysisData_t* model);

	// TODO- type check item- return indicates "hasError".
	bool doChecking(AnalysisData_t* model);

	// TODO- resolve last of (meta) data before translating.
	void resolveData(TranslationData_t* model);

	// TODO- get section item is in (most do, some don't).
	SectionType_e getSection(void);

	// TODO- get special label (unique to control items).
	std::string getLabel(void);

	// TODO- "to string" for ease of debugging.
	std::string asStr(void);

	// Std. destructor.
	~ControlItem(void);

private:
	// Typical directive information.
	SyntaxToken_e m_type; // type of directive (limited to control directives)
	std::string m_label;   // name of label being given
};

#endif /* SRC_DSITEMS_CONTROLITEM_H_ */
