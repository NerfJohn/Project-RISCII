/*
 * FuncNode.h: Node representing a function (declaration or definition).
 */

#ifndef DS_FUNCNODE_H_
#define DS_FUNCNODE_H_

#include <stack>
#include "Ds/IAstNode.h"

/*
 * @brief Representation of head of function- defined or declared.
 */
class FuncNode: public IAstNode {
public:
	/*
	 * @brief Constructor- creates node from action stack items.
	 *
	 * Assumes action stack has been grammar checked/parsed for correctness.
	 * Intended to be used with Common/Device/Parser.
	 *
	 * @param actStack action stack with node's build items
	 */
	FuncNode(std::stack<Ptr<IBuildItem>>& actStack);

	/*
	 * @brief Std destructor- deletes underlying nodes/tokens.
	 */
	~FuncNode(void);

private:
	// Nodes/tokens pulled from action stack.
	std::deque<Ptr<LexToken>> m_typeTkns; // return type tokens
	Ptr<LexToken>             m_name;     // identifier
};

#endif /* DS_FUNCNODE_H_ */
