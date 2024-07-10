/*
 * IBuildItem.h
 *
 * "Interface defining necessary functions per assembly construct"
 */

#ifndef SRC_ITEMS_IBUILDITEM_H_
#define SRC_ITEMS_IBUILDITEM_H_

#include "DomainLayer/DataModel_t.h"

/*
 * Interface used to define all assembly language constructs/larger pieces.
 *
 * Interface allows program to store, iterate over, and call the items that
 * ultimately define the high-level assembly language features. The interface
 * primarily defines the necessary functions to check and translate items.
 */
class IBuildItem {
public:
	/*
	 * Performs analysis and checks available before entire program is parsed.
	 *
	 * Any checks or data analysis that doesn't require knowledge about other
	 * items or files is performed. Updates data model with any analyzed info
	 * (or found warnings/errors).
	 *
	 * @param model data model of the program
	 */
	virtual void doLocalAnalysis(DataModel_t* model) = 0;

	/*
	 * Performs analysis and checks with respect to the entire given program.
	 *
	 * Any checks or data analysis that require information about the entire
	 * given program (or just weren't done during doLocalAnalysis()) are done.
	 * Update model with analyzed info (or found warnings/errors).
	 *
	 * @param model data model of the program
	 */
	virtual void doGlobalAnalysis(DataModel_t* model) = 0;

	/*
	 * Std. destructor- no real use for interface.
	 */
	virtual ~IBuildItem(void) {}
};

#endif /* SRC_ITEMS_IBUILDITEM_H_ */
