/*
 * ABuildItem.h
 *
 *  Created on: Apr 1, 2024
 *      Author: John
 */

#ifndef SRC_DSITEMS_ABUILDITEM_H_
#define SRC_DSITEMS_ABUILDITEM_H_

#include <iostream>
#include "../Analysis/AnalysisData_t.h"
#include "../Analysis/TranslationData_t.h"
#include "SectionType_e.h"

// TODO- Abstract to define base for assembly line item.
class ABuildItem {
public:
	// TODO- analyze item for later checks and translation.
	virtual void doAnalysis(AnalysisData_t* model) = 0;

	// TODO- type check item- return indicates "hasError".
	virtual bool doChecking(AnalysisData_t* model) = 0;

	// TODO- resolve last of (meta) data before translating.
	virtual void resolveData(TranslationData_t* model) = 0;

	// TODO- get section item is in (most do, some don't).
	virtual SectionType_e getSection(void) = 0;

	// TODO- print option for sake of debugging.
	virtual std::string asStr(void) = 0;

	// TODO- read out file/line origin (namely for reporting).
	std::string getOrigin(void);

	// TODO- Std. destructor.
	virtual ~ABuildItem(void);

protected:
	// TODO- origin of item (by first token/char position).
	std::string m_origin;

	// TODO- address associated with item (sometime unused).
	uint16_t m_address;
};

#endif /* SRC_DSITEMS_ABUILDITEM_H_ */
