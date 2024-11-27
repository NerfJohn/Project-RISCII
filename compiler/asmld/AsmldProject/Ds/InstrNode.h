/*
 * InstrNode.h: Represents single ISA instruction in assembly.
 */

#ifndef DS_INSTRNODE_H_
#define DS_INSTRNODE_H_

#include <stack>
#include "Ds/AAsmNode.h"
#include "Ds/ItemToken.h"

/*
 * Class implementing single ISA based instructions (in the assembly language).
 *
 * Implements the checks/translations needed to assemble/link a binary
 * instruction. Derives functions from AAsmNode to allow for the node to be part
 * of a larger data structure, yet still be accessed appropriately.
 */
class InstrNode: public AAsmNode {
public:
	/*
	 * Constructor called by parser. Builds itself directly from action stack.
	 *
	 * Builds node from all items stored on the given stack. Constructor
	 * implements some checks on stack composition, but generally assumes the
	 * items have already been checked/parsed by a parser.
	 */
	InstrNode(std::stack<ItemToken*>& itemStack);

	/*
	 * Runs local analytics on node's data.
	 *
	 * Localized analysis generally involves checking arguments are semantically
	 * correct and managing local level symbol declaring/modifying/linking. This
	 * function cannot directly terminate the program, but can log errors.
	 *
	 * @param model shared data of the entire program
	 */
	void doLocalAnalysis(DataModel_t& model);

	/*
	 * Computes address-related data for model and node.
	 *
	 * Nodes that take up space in the binary image update the model to reflect
	 * their effect on addresses while nodes that use addresses use the model to
	 * determine their actual value.
	 *
	 * @param model shared data of the entire program
	 */
	void genAddresses(DataModel_t& model);

	/*
	 * Assembles the node, adding its binary data to the model.
	 *
	 * Nodes that take up space in the binary image generate and update the
	 * model with their binary equivalent. Function has no effect on all other
	 * nodes.
	 *
	 * @param model shared data of the entire program
	 */
	void genAssemble(DataModel_t& model);

private:
	// Raw items/tokens composing the instruction.
	ItemToken*               m_itemOp;   // MUST be non-null
	ItemToken*               m_itemFlag;
	std::deque<ItemToken*>   m_itemRegs;
	ItemToken*               m_itemImm;

	// Helper function to warn about repeated flags in instructions.
	std::string getRepeats(std::string const& str);

	// Helper function to extract "integer" values (quickly- minimal checks).
	void extractInts(std::vector<uint8_t>& regInts, int32_t& immInt);
};

#endif /* DS_INSTRNODE_H_ */
