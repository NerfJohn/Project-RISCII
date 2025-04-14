/*
 * IAstNode.h: Interface defining nodes with respect to program's larger steps.
 */

#ifndef DS_IASTNODE_H_
#define DS_IASTNODE_H_

#include "Common/Ds/IBuildItem.h"
#include "Domain/DataModel_t.h"

/*
 * @brief "Interface" class for nodes making up Abstract Syntax Tree (AST).
 *
 * Abstract class, but little/no real implementations. Intended to provide
 * way to conduct program steps for all nodes (via polymorphism).
 */
class IAstNode: public IBuildItem {
public:
	/*
	 * @brief Analyze AST, creating and linking symbols.
	 *
	 * Intended as "primer" for conduct primary checks on AST. Manages creation,
	 * scoping, and linking of all symbols.
	 *
	 * @param model data model of the entire program
	 */
	virtual void analyze(DataModel_t& model) {/* no actions */}

	/*
	 * @brief Std destructor.
	 */
	virtual ~IAstNode(void) {/* no actions */}

protected:
	// "Interface"- only 'created' by derived classes.
	IAstNode(void)  {/* no actions */}
};

#endif /* DS_IASTNODE_H_ */
