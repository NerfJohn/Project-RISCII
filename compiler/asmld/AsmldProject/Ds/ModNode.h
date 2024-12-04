/*
 * ModNode.h: Represents modifier of a label/named address.
 */

#ifndef DS_MODNODE_H_
#define DS_MODNODE_H_

#include <stack>
#include "Ds/AAsmNode.h"
#include "Ds/ItemToken.h"

/*
 * Build node used to represent a (locally declared) label modifier directive.
 *
 * Building node from items off the parser's action stack, implements functions
 * to modify symbols during local linking. Once done, lets itself deleted during
 * cleanup (ie modify and run).
 */
class ModNode: public AAsmNode {
public:
	// Enforce node class as "no copy".
	ModNode(ModNode const&)        = delete;
	void operator=(ModNode const&) = delete;

	/*
	 * Constructor called by parser. Builds itself directly from action stack.
	 *
	 * Builds node from ALL items stored on the given stack. Constructor
	 * implements some checks on stack composition, but generally assumes the
	 * items have ALREADY BEEN CHECKED by a parser.
	 */
	ModNode(std::stack<ItemToken*>& itemStack);

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
	 * Finishing program checks- requesting deletions as needed to slim program.
	 *
	 * Primarily acts as a way for state steps to remove nodes with no more use.
	 * Also acts as a last opportunity to check program for errors and warnings,
	 * such as unused symbols.
	 *
	 * @param model shared data of the entire program
	 * @return      CLEAN_DELETE if node can be deleted, CLEAN_KEEP otherwise
	 */
	CleanAction_e globalClean(DataModel_t& model);

	/*
	 * General destructor- ensures all memory is freed on deletion.
	 */
	~ModNode(void);

private:
	// Raw items REQUIRED to compose label modifier.
	ItemToken*               m_reqType;
	ItemToken*               m_reqLabel;
};

#endif /* DS_MODNODE_H_ */
