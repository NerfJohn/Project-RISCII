/*
 * DeclNode.h: Represents declaration of a label/named address.
 */

#ifndef DS_DECLNODE_H_
#define DS_DECLNODE_H_

#include <stack>
#include "Ds/AAsmNode.h"
#include "Ds/ItemToken.h"

/*
 * Build node representing local label declaration.
 *
 * Building node from item's off the parser's action stack, implements functions
 * to declare and manage label declarations. Creates a unique symbol per
 * declaration, ensuring it is defined and addressed correctly.
 */
class DeclNode: public AAsmNode {
public:
	// Enforce node class as "no copy".
	DeclNode(DeclNode const&)       = delete;
	void operator=(DeclNode const&) = delete;

	/*
	 * Constructor called by parser. Builds itself directly from action stack.
	 *
	 * Builds node from ALL items stored on the given stack. Constructor
	 * implements some checks on stack composition, but generally assumes the
	 * items have ALREADY BEEN CHECKED by a parser.
	 */
	DeclNode(std::stack<ItemToken*>& itemStack);

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
	 * General destructor- ensures all memory is freed on deletion.
	 */
	~DeclNode(void);

private:
	// Raw items REQUIRED to compose label declaration.
	ItemToken*               m_reqLabel;

	// Symbol created by declaration.
	Symbol_t*                m_sym;
};

#endif /* DS_DECLNODE_H_ */
