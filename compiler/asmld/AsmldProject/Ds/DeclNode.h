/*
 * DeclNode.h: Represents declaration of a label/named address.
 */

#ifndef DS_DECLNODE_H_
#define DS_DECLNODE_H_

#include <stack>
#include "Ds/AAsmNode.h"
#include "Ds/ItemToken.h"

/*
 * Class implementing a declaration of a labeled address.
 *
 * Implements checks/translations required to declare named addresses that can
 * be referred to by other nodes. Derives functions from AAsmNode to allow for
 * the node to be part of a larger data structure, yet still be accessed.
 */
class DeclNode: public AAsmNode {
public:
	/*
	 * Constructor called by parser. Builds itself directly from action stack.
	 *
	 * Builds node from all items stored on the given stack. Constructor
	 * implements some checks on stack composition, but generally assumes the
	 * items have already been checked/parsed by a parser.
	 */
	DeclNode(std::stack<ItemToken*>& itemStack);

	/*
	 * Runs local analytics on node's data.
	 *
	 * Localized analysis generally involves checking arguments are semantically
	 * correct and managing local level symbol declaring/modifying/linking. This
	 * function cannot directly terminate the program, but can log errors.
	 *
	 * @param model shared data of the entire program
	 * @param syms  symbol table of localized symbols
	 */
	void doLocalAnalysis(DataModel_t& model, SymTable& syms);

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
	// Raw items/tokens composing the declaration.
	ItemToken*               m_itemLabel;   // MUST be non-null

	// Symbol created/managed by declaration.
	Symbol_t*                m_sym;
};

#endif /* DS_DECLNODE_H_ */
