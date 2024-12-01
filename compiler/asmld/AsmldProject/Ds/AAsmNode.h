/*
 * AAsmNode.h: Abstract (interface) class for pieces of core data structure.
 */

#ifndef DS_AASMNODE_H_
#define DS_AASMNODE_H_

#include "Device/SymTable.h"
#include "Domain/DataModel_t.h"
#include "Ds/ItemToken.h"

/*
 * Abstract class used to give nodes a general interface for operations.
 *
 * Nodes, the building blocks of the core data structure, act as derived classes
 * of this abstract class. This ensures all node functions define necessary
 * functions, being able to be stored/called as generic build pieces.
 */
class AAsmNode {
public:
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
	virtual void doLocalAnalysis(DataModel_t& model, SymTable& syms);

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
	virtual void doLocalLinking(DataModel_t& model, SymTable& syms);

	/*
	 * Adds node to model's overall program data structures.
	 *
	 * Function focused on adding nodes and symbols to the model (as
	 * appropriate). Additional logic is performed to necessary errors/warnings.
	 * Should be done after each node has been locally analyzed/linked.
	 *
	 * @param model shared data of the entire program
	 */
	virtual void addToProgram(DataModel_t& model);

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
	virtual void genAddresses(DataModel_t& model);

	/*
	 * Assembles the node, adding its binary data to the model.
	 *
	 * Nodes that take up space in the binary image generate and update the
	 * model with their binary equivalent. Function has no effect on all other
	 * nodes.
	 *
	 * @param model shared data of the entire program
	 */
	virtual void genAssemble(DataModel_t& model);

protected:
	// Protect constructor/destructor to ensure class is abstract.
	         AAsmNode() {/* no actions */}
	virtual ~AAsmNode() {/* no actions */}

	// Helper function for common immediate value validation.
	virtual void validateImm(DataModel_t& model,
			                 ItemToken const& immItem,
					         ItemToken const& immOp);

	// Helper function for common immediate value extraction.
	virtual int32_t getImmVal(ItemToken const& immItem);

	// Helper function for pairing nodes to labels.
	virtual void pairOpenLabels(DataModel_t& model,
			                    ItemToken& fileLoc,
								AddrSpace_e const space);

	// Helper function for freeing symbols upon destruction.
	virtual void freeSymbol(Symbol_t*& sym);
};

#endif /* DS_AASMNODE_H_ */
