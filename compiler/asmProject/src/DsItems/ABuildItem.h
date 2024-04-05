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

// TODO- Abstract to define base for assembly line item.
class ABuildItem {
public:
	// TODO- analyze item for later checks and translation.
	virtual void doAnalysis(AnalysisData_t* model) = 0;

	// TODO- print option for sake of debugging.
	virtual std::string asStr(void) = 0;

	// TODO- read out file/line origin (namely for reporting).
	std::string getOrigin(void);

	// TODO- Std. destructor.
	virtual ~ABuildItem(void);

protected:
	// TODO- origin of item (by first token/char position).
	std::string m_origin;
};

#endif /* SRC_DSITEMS_ABUILDITEM_H_ */
