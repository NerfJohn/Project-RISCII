/*
 * DataNode.h: Represents directives specifying global data (data and bss).
 */

#ifndef DS_DATANODE_H_
#define DS_DATANODE_H_

#include <stack>
#include "Ds/AAsmNode.h"
#include "Ds/ItemToken.h"

/*
 * Build node representing a data allocation directive.
 *
 * Building node from items off the parser's action stack, implements functions
 * to analyze and generate allocated data. Handles both initialized and
 * uninitialized data allocation/organization.
 */
class DataNode: public AAsmNode {
public:
	// Enforce node class as "no copy".
	DataNode(DataNode const&)       = delete;
	void operator=(DataNode const&) = delete;

	/*
	 * Constructor called by parser. Builds itself directly from action stack.
	 *
	 * Builds node from ALL items stored on the given stack. Constructor
	 * implements some checks on stack composition, but generally assumes the
	 * items have ALREADY BEEN CHECKED by a parser.
	 */
	DataNode(std::stack<ItemToken*>& itemStack);

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
	 * Handles local links/symbols- modifying and linking to local symbols.
	 *
	 * Primarily links references to local symbols as applicable. Also applies
	 * symbol modifiers and pairing checks, ensuring all local symbol info is
	 * accounted for and linked as able.
	 *
	 * @param model shared data of the entire program
	 * @param table record of locally declared symbols
	 */
	void localLink(DataModel_t& model, SymTable& table);

	/*
	 * Handles global links/symbols- finishing overall symbol linkage.
	 *
	 * Finishes work of AAsmNode::localLink() by linking remaining references
	 * to symbols/labels. Also handles re-linking weak symbols/references as
	 * applicable.
	 *
	 * @param model shared data of the entire program
	 */
	void globalLink(DataModel_t& model);

	/*
	 * Analyze program- generating addresses for each symbol.
	 *
	 * Primarily calculates addresses for each symbol. Also computes program
	 * sizes for state step checks. Assumes program nodes are being called in
	 * the order they will be assembled/placed.
	 *
	 * @param model shared data of the entire program
	 */
	void imageAddress(DataModel_t& model);

	/*
	 * Assembles program- generating binary values in the data model.
	 *
	 * Generates and stores binary values in the data model. Assumes program
	 * nodes are being called in the order they will be placed into the file.
	 *
	 * @param model shared data of the entire program
	 */
	void imageAssemble(DataModel_t& model);

	/*
	 * General destructor- ensures all memory is freed on deletion.
	 */
	~DataNode(void);

private:
	// Raw items REQUIRED to compose data directive.
	ItemToken*               m_reqType;

	// Raw items OPTIONAL/MULTI-ELEMENT to compose directive.
	std::deque<ItemToken*>   m_optVals; // undo "stack inversion" of values

	// Symbols tied to stored labels (in same order as labels in values).
	std::vector<Symbol_t*>   m_syms;

	// Helper function to determine bytes to allocate using immediate.
	uint32_t allocImm(DataModel_t& model,
			          ItemToken const& imm,
					  ItemToken const& type);
};

#endif /* DS_DATANODE_H_ */
