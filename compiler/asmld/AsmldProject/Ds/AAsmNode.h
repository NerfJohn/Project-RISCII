/*
 * AAsmNode.h: Abstract (interface) class for pieces of core data structure.
 */

#ifndef DS_AASMNODE_H_
#define DS_AASMNODE_H_

/*
 * Abstract class used to give nodes a general interface for operations.
 *
 * Nodes, the building blocks of the core data structure, act as derived classes
 * of this abstract class. This ensure all node functions define necessary
 * functions, being able to be stored/called as generic build pieces.
 */
class AAsmNode {
protected:
	// Protect constructor/destructor to ensure class is abstract.
	         AAsmNode() {/* no actions */}
	virtual ~AAsmNode() {/* no actions */}
};

#endif /* DS_AASMNODE_H_ */
