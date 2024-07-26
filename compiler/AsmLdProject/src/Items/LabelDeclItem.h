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
 * TODO
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
