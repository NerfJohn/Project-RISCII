/*
 * FileNode.h: Node representing entire file/translation unit.
 */

#ifndef DS_FILENODE_H_
#define DS_FILENODE_H_

#include <stack>
#include "Ds/IAstNode.h"

/*
 * @brief Representation of one file/translation unit. Head/root of AST.
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
	FileNode(std::stack<Ptr<IBuildItem>>& actStack);

	/*
	 * @brief Analyzes node- prepping information to be checked by check().
	 *
	 * Primarily creates/links symbols while also translating inherited tokens
	 * into useful, comparable values.
	 *
	 * @param model data model of the entire program
	 */
	void analyze(DataModel_t& model);

	// TODO
	Ptr<Type_t> check(DataModel_t& model);

	/*
	 * @brief Std destructor- deletes underlying nodes/tokens.
	 */
	~FileNode(void);

private:
	// Nodes/tokens pulled from action stack.
	std::deque<Ptr<IAstNode>> m_elements; // larger program pieces
};

#endif /* DS_FILENODE_H_ */
