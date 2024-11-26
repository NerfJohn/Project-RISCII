/*
 * AAsmNode.h: Abstract (interface) class for pieces of core data structure.
 */

#ifndef DS_AASMNODE_H_
#define DS_AASMNODE_H_

#include "Domain/DataModel_t.h"
#include "Domain/RetErr_e.h"

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
	 */
	virtual void doLocalAnalysis(DataModel_t& model) = 0;

	// TODO
	virtual void assemblePrgm(DataModel_t& model) = 0;

protected:
	// Protect constructor/destructor to ensure class is abstract.
	         AAsmNode() {/* no actions */}
	virtual ~AAsmNode() {/* no actions */}
};

#endif /* DS_AASMNODE_H_ */
