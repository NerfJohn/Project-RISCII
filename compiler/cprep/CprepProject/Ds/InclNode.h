/*
 * InclNode.h: Node representing file inclusion.
 */

#ifndef DS_INCLNODE_H_
#define DS_INCLNODE_H_

#include <stack>
#include "Ds/IAstNode.h"

/*
 * @brief Represents file inclusion (ie copy file contents here).
 *
 * Searches in node's origin file directory, then model's include directories
 * (in order they're stored).
 */
class InclNode: public IAstNode {
public:
	/*
	 * @brief Constructor- creates node from action stack items.
	 *
	 * Assumes action stack has been grammar checked/parsed for correctness.
	 * Intended to be used with Common/Device/Parser.
	 *
	 * @param actStack action stack with node's build items
	 */
	InclNode(std::stack<IBuildItem*>& actStack);

	/*
	 * @brief Std destructor- deletes underlying nodes/tokens.
	 */
	~InclNode(void);

private:
	// "Required" items- checked for correctness on creation.
	std::string m_reqFname;
};

#endif /* DS_INCLNODE_H_ */
