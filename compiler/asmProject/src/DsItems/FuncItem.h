/*
 * FuncItem.h
 *
 *  Created on: Apr 3, 2024
 *      Author: John
 */

#ifndef SRC_DSITEMS_FUNCITEM_H_
#define SRC_DSITEMS_FUNCITEM_H_

#include <stack>
#include "ABuildItem.h"
#include "../Parsing/AsmToken.h"

// TODO- Just implements "_la"- rest are derivatives of this.

// TODO- represents built-in functions (ie multi instruction steps).
class FuncItem: public ABuildItem {
public:
	// TODO- instances created from tokens on action stack.
	FuncItem(std::stack<AsmToken*> actStack);

	// TODO- "to string" for ease of debugging.
	std::string asStr(void);

	// Std. destructor.
	~FuncItem(void);

private:
	// Typical function information (TODO- just _la).
	int m_reg;           // register being written to
	std::string m_label; // label being written into register
};

#endif /* SRC_DSITEMS_FUNCITEM_H_ */
