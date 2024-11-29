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
 * of this abstract class. This ensure all node functions define necessary
 * functions, being able to be stored/called as generic build pieces.
 */
class AAsmNode {
public:
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
	virtual void doLocalAnalysis(DataModel_t& model, SymTable& syms) = 0;

	/*
	 * Computes address-related data for model and node.
	 *
	 * Nodes that take up space in the binary image update the model to reflect
	 * their effect on addresses while nodes that use addresses use the model to
	 * determine their actual value.
	 *
	 * @param model shared data of the entire program
	 */
	virtual void genAddresses(DataModel_t& model) = 0;

	/*
	 * Assembles the node, adding its binary data to the model.
	 *
	 * Nodes that take up space in the binary image generate and update the
	 * model with their binary equivalent. Function has no effect on all other
	 * nodes.
	 *
	 * @param model shared data of the entire program
	 */
	virtual void genAssemble(DataModel_t& model) = 0;

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
};

#endif /* DS_AASMNODE_H_ */
