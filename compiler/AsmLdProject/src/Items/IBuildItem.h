/*
 * IBuildItem.h
 *
 * "Interface defining necessary functions per assembly construct"
 */

#ifndef SRC_ITEMS_IBUILDITEM_H_
#define SRC_ITEMS_IBUILDITEM_H_

#include "DomainLayer/DataModel_t.h"
#include "DomainLayer/RetErr_e.h"

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
	 * Translates item's values into binary words. Populates model's sections.
	 *
	 * Has the item convert its contents to its binary representation (as used
	 * in the binary image). Once converted, it appends the converted values to
	 * the proper section in the data model for writing to the image file.
	 *
	 * @param model data model of the program
	 */
	virtual void generateBinaryValue(DataModel_t* model) = 0;

	/*
	 * Returns the generated address of item (if item has address).
	 *
	 * Addresses are "software" addresses generated by items after
	 * generateBinaryValue() is called. Value and associated address space are
	 * based on the type of item. Return code indicates if item has an address.
	 */
	virtual RetErr_e getAddress(uint32_t& addr) = 0;

	/*
	 * Std. destructor- no real use for interface.
	 */
	virtual ~IBuildItem(void) {}
};

#endif /* SRC_ITEMS_IBUILDITEM_H_ */
