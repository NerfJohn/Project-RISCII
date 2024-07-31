/*
 * LabelDeclItem.h
 *
 * "Item defining a declaration of a labeled address/item"
 */

#ifndef SRC_ITEMS_LABELDECLITEM_H_
#define SRC_ITEMS_LABELDECLITEM_H_

#include <queue>
#include "DomainLayer/ScanToken_t.h"
#include "Items/IBuildItem.h"

/*
 * Class to hold data and functions related to label declarations.
 *
 * Created from parser output, the label declaration item implements checks
 * and functions needed to implement labels as abstracted addresses. Class
 * assumes it is the final owner of the given scan tokens.
 */
class LabelDeclItem: public IBuildItem {
public:
	/*
	 * Constructs label declaration item from valid, parsed tokens.
	 *
	 * Uses ordered tokens to initialize it's data, but assumes the tokens
	 * ordering + contents have already been validated for the item.
	 * Item takes hard ownership of tokens (ie cannot pass on ownership).
	 *
	 * @param tokens parsed and verified tokens for a label declaration item
	 */
	LabelDeclItem(std::queue<ScanToken_t*>* tokens);

	/*
	 * Performs analysis and checks available before entire program is parsed.
	 *
	 * Any checks or data analysis that doesn't require knowledge about other
	 * items or files is performed. Updates data model with any analyzed info
	 * (or found warnings/errors).
	 *
	 * @param model data model of the program
	 */
	void doLocalAnalysis(DataModel_t* model);

	/*
	 * Performs analysis and checks with respect to the entire given program.
	 *
	 * Any checks or data analysis that require information about the entire
	 * given program (or just weren't done during doLocalAnalysis()) are done.
	 * Update model with analyzed info (or found warnings/errors).
	 *
	 * @param model data model of the program
	 */
	void doGlobalAnalysis(DataModel_t* model);

	/*
	 * Confirms if item's contents are placed in text section of binary.
	 *
	 * Indicates if the item is used to generate values for the text section
	 * (ie generates instructions). Intended use is to determine if a label is
	 * referencing the program address space (vs data address space).
	 *
	 * @return true if text section content, false otherwise
	 */
	bool isTextContent(void);

	/*
	 * Translates item's values into binary words. Populates model's sections.
	 *
	 * Has the item convert its contents to its binary representation (as used
	 * in the binary image). Once converted, it appends the converted values to
	 * the proper section in the data model for writing to the image file.
	 *
	 * @param model data model of the program
	 */
	void generateBinaryValue(DataModel_t* model);

	/*
	 * Resolves any binary generations reliant on label resolution.
	 *
	 * Intended to be run after generateBinaryValue() and ultimately finish the
	 * process of binary translation. Focuses on resolving labels (which require
	 * information generated by generateBinaryValue().
	 *
	 * @param model data model of the program
	 */
	void resolveBinaryLabels(DataModel_t& model);

	/*
	 * Returns the generated address of item (if item has address).
	 *
	 * Addresses are "software" addresses generated by items after
	 * generateBinaryValue() is called. Value and associated address space are
	 * based on the type of item. Return code indicates if item has an address.
	 *
	 * @param addr location to populate with address
	 * @return zero if successful/has address ,non-zero otherwise.
	 */
	RetErr_e getAddress(uint32_t& addr);

	/*
	 * Destructs label declaration item (along with originally passed tokens).
	 *
	 * Item assumes hard ownership of the items- using the saved pointer to
	 * delete the scan token instances when the item item is deleted.
	 */
	~LabelDeclItem(void);

private:
	// Raw components of a label declaration.
	ScanToken_t* m_label;
};

#endif /* SRC_ITEMS_LABELDECLITEM_H_ */
