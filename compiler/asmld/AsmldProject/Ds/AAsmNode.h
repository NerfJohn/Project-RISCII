/*
 * AAsmNode.h: Abstract (interface) class for pieces of core data structure.
 */

#ifndef DS_AASMNODE_H_
#define DS_AASMNODE_H_

#include "Domain/CleanAction_e.h"
#include "Domain/DataModel_t.h"
#include "Ds/ItemToken.h"

/*
 * Abstract class used to define nodes used to build the program.
 *
 * Defines common/necessary functions to read/process/generate the program.
 * Also defines protected helper functions common to multiple derived node types
 * for convenience.
 */
class AAsmNode {
public:
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
	virtual void localAnalyze(DataModel_t& model, SymTable& table);

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
	virtual void localLink(DataModel_t& model, SymTable& table);

	/*
	 * Handles global links/symbols- finishing overall symbol linkage.
	 *
	 * Finishes work of AAsmNode::localLink() by linking remaining references
	 * to symbols/labels. Also handles re-linking weak symbols/references as
	 * applicable.
	 *
	 * @param model shared data of the entire program
	 */
	virtual void globalLink(DataModel_t& model);

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
	virtual CleanAction_e globalClean(DataModel_t& model);

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
	 * Writes debug table information to (assumed open) binary image.
	 *
	 * Assuming the binary image is open for writing, writes relevant info to
	 * the file (in the form of a debug table entry). Intended as an optional
	 * feature of the assembly/linking process.
	 */
	virtual void optPrintDebug(void);

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
	virtual CleanAction_e optRemoveLabel(DataModel_t& model);

	/*
	 * General destructor- public to allow for generic node deletion.
	 */
	virtual ~AAsmNode(void);

protected:
	// Protect constructor to ensure class is abstract.
	AAsmNode() {/* no actions */}

	// Common helper function to get/validate register value.
	uint8_t getReg(DataModel_t& model, ItemToken const& reg);

	// Common helper function to get/validate immediate values.
	int32_t getImm(DataModel_t& model,
			       ItemToken const& imm,
				   ItemToken const& op);

	// Common helper function to claim unpaired labels.
	void pairLabels(DataModel_t& model,
			        ItemToken const& loc,
					AddrSpace_e const space);

	// Common helper function to link a local label, finding its symbol.
	void linkLocal(SymTable& table,
			       ItemToken const& label,
				   Symbol_t*& sym);

	// Common helper function to link a global label, finding its symbol.
	void linkGlobal(DataModel_t& model,
			        ItemToken const& label,
					Symbol_t*& sym);

	// Common helper function to free symbols/ptrs to symbols.
	void freeSymbol(Symbol_t*& sym);
};

#endif /* DS_AASMNODE_H_ */
