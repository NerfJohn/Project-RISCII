/*
 * InstrNode.h: Represents single ISA instruction in assembly.
 */

#ifndef DS_INSTRNODE_H_
#define DS_INSTRNODE_H_

#include <stack>
#include "Ds/AAsmNode.h"
#include "Ds/ItemToken.h"

/*
 * Build node used to represent an ISA instruction.
 *
 * Building node from items off the parser's action stack, implements functions
 * to analyze and generate binary instructions. Ensures arguments semantically
 * fit into instruction (as defined by the ISA).
 */
class InstrNode: public AAsmNode {
public:
	// Enforce node class as "no copy".
	InstrNode(InstrNode const&)      = delete;
	void operator=(InstrNode const&) = delete;

	/*
	 * Constructor called by parser. Builds itself directly from action stack.
	 *
	 * Builds node from ALL items stored on the given stack. Constructor
	 * implements some checks on stack composition, but generally assumes the
	 * items have ALREADY BEEN CHECKED by a parser.
	 */
	InstrNode(std::stack<ItemToken*>& itemStack);

	/*
	 * Analyzes node- validating local arguments/symbols.
	 *
	 * Validates node's arguments- ensuring value are semantically correct. Also
	 * declares/creates symbols and pairs them with their target nodes/address
	 * spaces.
	 *
	 * @param model shared data of the entire program
	 * @param table record of locally declared symbols
	 */
	void localAnalyze(DataModel_t& model, SymTable& table);

	/*
	 * Analyze program- generating addresses for each symbol.
	 *
	 * Primarily calculates addresses for each symbol. Also computes program
	 * sizes for state step checks. Assumes program nodes are being called in
	 * the order they will be assembled/placed.
	 *
	 * @param model shared data of the entire program
	 */
	virtual void imageAddress(DataModel_t& model);

	/*
	 * Assembles program- generating binary values in the data model.
	 *
	 * Generates and stores binary values in the data model. Assumes program
	 * nodes are being called in the order they will be placed into the file.
	 *
	 * @param model shared data of the entire program
	 */
	virtual void imageAssemble(DataModel_t& model);

	/*
	 * Determines if node (+related section nodes) should be removed.
	 *
	 * Updates model for removing unused sections of the program. Also returns
	 * to indicate if node itself should be removed. Intended as an optional
	 * feature of the assembly/linking process.
	 *
	 * @param model shared data of the entire program.
	 * @return      CLEAN_DELETE if node should be deleted, CLEAN_KEEP otherwise
	 */
	CleanAction_e optRemoveLabel(DataModel_t& model);

	/*
	 * General destructor- ensures all memory is freed on deletion.
	 */
	~InstrNode(void);

private:
	// Raw items REQUIRED to compose instruction.
	ItemToken*               m_reqOp;

	// Raw items OPTIONAL/MULTI-ELEMENT to compose instruction.
	ItemToken*               m_optFlag;
	std::deque<ItemToken*>   m_optRegs; // undo "stack inversion" of regs
	ItemToken*               m_optImm;

	// Helper function to warn about repeated flags in instruction.
	std::string getRepeats(std::string const& str);
};

#endif /* DS_INSTRNODE_H_ */
