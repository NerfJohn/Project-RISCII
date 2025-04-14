/*
 * FileNode.h: Node representing an entire file of source code.
 */

#ifndef DS_FILENODE_H_
#define DS_FILENODE_H_

#include <stack>
#include "Common/Ds/LexToken.h"
#include "Domain/Type_t.h"
#include "Ds/IAstNode.h"

/*
 * @brief Representation of entire translation module. Top of AST.
 *
 * Mostly just directs calls towards children. Useful to act as single "top"
 * node of AST.
 */
class FileNode: public IAstNode {
public:
	/*
	 * @brief Constructor- creates node from action stack items.
	 *
	 * Assumes action stack has been grammar checked/parsed for correctness.
	 * Intended to be used with Common/Device/Parser.
	 *
	 * @param actStack action stack with node's build items
	 */
	FileNode(std::stack<IBuildItem*>& actStack);

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
	~FileNode(void);

private:
	// "Required" items- checked for correctness on creation.
	std::deque<IAstNode*> m_decls;
};

#endif /* DS_FILENODE_H_ */
