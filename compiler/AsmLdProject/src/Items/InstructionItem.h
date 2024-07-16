/*
 * InstructionItem.h
 *
 * "Item defining a binary instruction written in assembly"
 */

#ifndef SRC_ITEMS_INSTRUCTIONITEM_H_
#define SRC_ITEMS_INSTRUCTIONITEM_H_

#include <queue>
#include "DomainLayer/ScanToken_t.h"
#include "Items/IBuildItem.h"

/*
 * Class to hold data and functions related to binary instructions.
 *
 * Created from a parser's output, the instruction item implements the functions
 * necessary to check and translate the given tokens into a binary instruction.
 * Class assumes it is the final owner of the given scan tokens.
 */
class InstructionItem: public IBuildItem {
public:
	/*
	 * Constructs instruction item from parsed tokens. Assumes tokens are valid.
	 *
	 * Uses ordered tokens to initialize it's data, but assumes the tokens
	 * ordering + contents have already been validated for an instruction item.
	 * Item takes hard ownership of tokens (ie cannot pass on ownership).
	 *
	 * @param tokens parsed and verified tokens for an instruction item
	 */
	InstructionItem(std::queue<ScanToken_t*>* tokens);

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
	 * Destructs instruction item (along with originally passed scan tokens).
	 *
	 * Item assumes hard ownership of the items- using the saved pointer to
	 * delete the scan token instances when the item item is deleted.
	 */
	~InstructionItem(void);

private:
	// Components of the instruction (in rawest form).
	ScanToken_t*              m_opcode;
	ScanToken_t*              m_flag;
	std::vector<ScanToken_t*> m_regs;
	ScanToken_t*              m_immediate;
};

#endif /* SRC_ITEMS_INSTRUCTIONITEM_H_ */
