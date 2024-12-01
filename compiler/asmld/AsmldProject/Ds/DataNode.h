/*
 * DataNode.h: Represents directives specifying global data (data and bss).
 */

#ifndef DS_DATANODE_H_
#define DS_DATANODE_H_

#include <stack>
#include "Ds/AAsmNode.h"
#include "Ds/ItemToken.h"

/*
 * Class implementing directive controlling global data layout of the program.
 *
 * Implements checks/translations needed to organize/assemble/link global data
 * for a program (initialized of not). Derives functions from AAsmNode to allow
 * node to be part of a larger data structure, yet still be accessed.
 */
class DataNode: public AAsmNode {
public:
	// Enforce node class as "no copy".
	DataNode(DataNode const&)       = delete;
	void operator=(DataNode const&) = delete;

	/*
	 * Constructor called by parser. Builds itself directly from action stack.
	 *
	 * Builds node from all items stored on the given stack. Constructor
	 * implements some checks on stack composition, but generally assumes the
	 * items have already been checked/parsed by a parser.
	 */
	DataNode(std::stack<ItemToken*>& itemStack);

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
	~DataNode(void);

private:
	// Raw items/tokens composing the directive.
	ItemToken*               m_itemType;   // MUST be non-null
	std::deque<ItemToken*>   m_itemVals;
};

#endif /* DS_DATANODE_H_ */
