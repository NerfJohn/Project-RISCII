/*
 * FuncNode.h: Represents "load address" function (built-in 2 instruction op).
 */

#ifndef DS_FUNCNODE_H_
#define DS_FUNCNODE_H_

#include <stack>
#include "Ds/AAsmNode.h"
#include "Ds/ItemToken.h"

// TODO
class FuncNode: public AAsmNode {
public:
	// Enforce node class as "no copy".
	FuncNode(FuncNode const&)        = delete;
	void operator=(FuncNode const&)  = delete;

	/*
	 * Constructor called by parser. Builds itself directly from action stack.
	 *
	 * Builds node from ALL items stored on the given stack. Constructor
	 * implements some checks on stack composition, but generally assumes the
	 * items have ALREADY BEEN CHECKED by a parser.
	 */
	FuncNode(std::stack<ItemToken*>& itemStack);

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
	~FuncNode(void);

private:
	// Raw items REQUIRED to compose function.
	ItemToken*               m_reqOp;
	ItemToken*               m_reqReg;
	ItemToken*               m_reqValue;

	// Symbol tied to stored label (as applicable).
	Symbol_t*                m_sym;
};

#endif /* DS_FUNCNODE_H_ */
