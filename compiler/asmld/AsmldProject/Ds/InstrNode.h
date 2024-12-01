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
	// Enforce node class as "no copy".
	InstrNode(InstrNode const&)      = delete;
	void operator=(InstrNode const&) = delete;

	/*
	 * Constructor called by parser. Builds itself directly from action stack.
	 *
	 * Builds node from all items stored on the given stack. Constructor
	 * implements some checks on stack composition, but generally assumes the
	 * items have already been checked/parsed by a parser.
	 */
	InstrNode(std::stack<ItemToken*>& itemStack);

	/*
	 * Runs local analytics and record-keeping on node's data.
	 *
	 * Localized analysis focused on checking arguments are semantically correct
	 * and tracking locally declared symbols/labels. Intended as the first step
	 * of file-level analysis.
	 *
	 * @param model shared data of the entire program
	 * @param syms  symbol table of localized symbols
	 */
	void doLocalAnalysis(DataModel_t& model, SymTable& syms);

	/*
	 * Adds node to model's overall program data structures.
	 *
	 * Function focused on adding nodes and symbols to the model (as
	 * appropriate). Additional logic is performed to necessary errors/warnings.
	 * Should be done after each node has been locally analyzed/linked.
	 *
	 * @param model shared data of the entire program
	 */
	void addToProgram(DataModel_t& model);

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

	/*
	 * General destructor- ensures all memory is freed on deletion.
	 */
	~InstrNode(void);

private:
	// Raw items/tokens composing the instruction.
	ItemToken*               m_itemOp;   // MUST be non-null
	ItemToken*               m_itemFlag;
	std::deque<ItemToken*>   m_itemRegs;
	ItemToken*               m_itemImm;

	// Helper function to warn about repeated flags in instructions.
	std::string getRepeats(std::string const& str);

	// Helper function to extract register values (quickly- minimal checks).
	void extractRegs(std::vector<uint8_t>& regInts);
};

#endif /* DS_INSTRNODE_H_ */
