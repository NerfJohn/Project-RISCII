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
	// Enforce node class as "no copy".
	DeclNode(DeclNode const&)       = delete;
	void operator=(DeclNode const&) = delete;

	/*
	 * Constructor called by parser. Builds itself directly from action stack.
	 *
	 * Builds node from all items stored on the given stack. Constructor
	 * implements some checks on stack composition, but generally assumes the
	 * items have already been checked/parsed by a parser.
	 */
	DeclNode(std::stack<ItemToken*>& itemStack);

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
	 * Runs last (iterable) checks on global program prior to modification.
	 *
	 * Executes any last actions (on node of global program) before program is
	 * considered ready to modifications or translation. Should be done after
	 * each node in global program has been globally linked.
	 *
	 * @param model shared data of the entire program
	 */
	virtual void cleanProgram(DataModel_t& model);

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
	 * General destructor- ensures all memory is freed on deletion.
	 */
	~DeclNode(void);

private:
	// Raw items/tokens composing the declaration.
	ItemToken*               m_itemLabel;   // MUST be non-null

	// Symbol created/managed by declaration.
	Symbol_t*                m_sym;
};

#endif /* DS_DECLNODE_H_ */
