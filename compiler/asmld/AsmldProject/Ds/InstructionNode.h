/*
 * InstructionNode.h: Represents single ISA instruction in assembly.
 */

#ifndef DS_INSTRUCTIONNODE_H_
#define DS_INSTRUCTIONNODE_H_

#include <vector>
#include "Domain/BuildStack_t.h"
#include "Ds/AAsmNode.h"
#include "Ds/ItemToken.h"

/*
 * Class implementing single ISA based instructions (in the assembly language).
 *
 * Implements the checks/translations needed to assemble/link a binary
 * instruction. Derives functions from AAsmNode to allow for the node to be part
 * of a larger data structure, yet still be accessed appropriately.
 */
class InstructionNode: public AAsmNode {
public:
	/*
	 * Constructor called by parser. Builds itself directly from action stack.
	 *
	 * Builds node from all items stored on the given stack. Constructor
	 * implements some checks on stack composition, but generally assumes the
	 * items have already been checked/parsed by a parser.
	 */
	InstructionNode(BuildStack_t& itemStack);

private:
	// Raw items/tokens composing the instruction.
	std::shared_ptr<ItemToken>              m_itemOp;   // MUST be non-null
	std::shared_ptr<ItemToken>              m_itemFlag;
	std::vector<std::shared_ptr<ItemToken>> m_itemRegs;
	std::shared_ptr<ItemToken>              m_itemImm;

};

#endif /* DS_INSTRUCTIONNODE_H_ */
