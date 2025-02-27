/*
 * DefNode.h: Node representing preprocessor (only) variable definition.
 */

#ifndef DS_DEFNODE_H_
#define DS_DEFNODE_H_

#include <stack>
#include "Ds/IAstNode.h"

/*
 * @brief Represents preprocessor variable definition.
 *
 * Basic definition only (ie no macro or copy/paste support).
 */
class DefNode: public IAstNode {
public:
	/*
	 * @brief Constructor- creates node from action stack items.
	 *
	 * Assumes action stack has been grammar checked/parsed for correctness.
	 * Intended to be used with Common/Device/Parser.
	 *
	 * @param actStack action stack with node's build items
	 */
	DefNode(std::stack<IBuildItem*>& actStack);

	/*
	 * @brief Std destructor- deletes underlying nodes/tokens.
	 */
	~DefNode(void);

private:
	// "Required" items- checked for correctness on creation.
	std::string m_reqDef;
};

#endif /* DS_DEFNODE_H_ */
