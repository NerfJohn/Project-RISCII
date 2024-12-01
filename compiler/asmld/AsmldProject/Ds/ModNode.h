/*
 * ModNode.h: Represents modifier of a label/named address.
 */

#ifndef DS_MODNODE_H_
#define DS_MODNODE_H_

#include <stack>
#include "Ds/AAsmNode.h"
#include "Ds/ItemToken.h"

/*
 * Class implementing modifier for a local label declaration.
 *
 * Implements checks/translations needed to support a modifier for a locally
 * declared label. Derives functions from AAsmNode to allow for the node to be
 * part of a larger data structure, yet still be accessed as needed.
 */
class ModNode: public AAsmNode {
public:
	// Enforce node class as "no copy".
	ModNode(ModNode const&)        = delete;
	void operator=(ModNode const&) = delete;

	/*
	 * Constructor called by parser. Builds itself directly from action stack.
	 *
	 * Builds node from all items stored on the given stack. Constructor
	 * implements some checks on stack composition, but generally assumes the
	 * items have already been checked/parsed by a parser.
	 */
	ModNode(std::stack<ItemToken*>& itemStack);

	/*
	 * Analyzes and links labels/symbols at the local level.
	 *
	 * Function focused on gathering additional information on locally declared
	 * labels/symbols, including linking localized references to their declared
	 * symbol. Should be done after each node has been locally analyzed.
	 *
	 * @param model shared data of the entire program
	 * @param syms  symbol table of localized symbols
	 */
	void doLocalLinking(DataModel_t& model, SymTable& syms);

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
	 * General destructor- ensures all memory is freed on deletion.
	 */
	~ModNode(void);

private:
	// Raw items/tokens composing the modifier.
	ItemToken*               m_itemType;   // MUST be non-null
	ItemToken*               m_itemLabel;  // MUST be non-null
};

#endif /* DS_MODNODE_H_ */
