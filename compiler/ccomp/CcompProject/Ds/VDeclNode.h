/*
 * VDeclNode.h: Node representing variable declaration.
 */

#ifndef DS_VDECLNODE_H_
#define DS_VDECLNODE_H_

#include <stack>
#include "Common/Ds/LexToken.h"
#include "Domain/Type_t.h"
#include "Ds/IAstNode.h"

/*
 * @brief Represents a variable declaration (local or global).
 *
 * Contains both global/local rules. Stores child LitNode if declaration is
 * initialized.
 */
class VDeclNode: public IAstNode {
public:
	/*
	 * @brief Constructor- creates node from action stack items.
	 *
	 * Assumes action stack has been grammar checked/parsed for correctness.
	 * Intended to be used with Common/Device/Parser.
	 *
	 * @param actStack action stack with node's build items
	 * @param model    data model of the entire program
	 */
	VDeclNode(std::stack<IBuildItem*>& actStack, DataModel_t& model);

	/*
	 * @brief Analyze AST, creating and linking symbols.
	 *
	 * Intended as "primer" for conduct primary checks on AST. Manages creation,
	 * scoping, and linking of all symbols.
	 *
	 * @param model data model of the entire program
	 */
	void analyze(DataModel_t& model);

	/*
	 * @brief Std destructor- deletes underlying nodes/tokens.
	 */
	~VDeclNode(void);

private:
	// "Required" items- checked for correctness on creation.
	Type_t                m_cType;
	LexToken*             m_id;

	// Optional items created during/after creation.
	LexToken*             m_arrSize;
	IAstNode*             m_initVal;
};

#endif /* DS_VDECLNODE_H_ */
