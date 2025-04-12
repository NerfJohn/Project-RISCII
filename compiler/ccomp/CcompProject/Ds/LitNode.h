/*
 * LitNode.h: Node representing literal/constant value.
 */

#ifndef DS_LITNODE_H_
#define DS_LITNODE_H_

#include <stack>
#include "Common/Ds/LexToken.h"
#include "Domain/Type_t.h"
#include "Ds/IAstNode.h"

/*
 * @brief Representation of literal value (scalar or array).
 *
 * Contains literal values + typing for all. Type specific only to whole (ie
 * details of either one value or one array).
 */
class LitNode: public IAstNode {
public:
	/*
	 * @brief Constructor- creates node from action stack items.
	 *
	 * Assumes action stack has been grammar checked/parsed for correctness.
	 * Intended to be used with Common/Device/Parser.
	 *
	 * @param actStack action stack with node's build items
	 */
	LitNode(std::stack<IBuildItem*>& actStack);

	/*
	 * @brief Std destructor- deletes underlying nodes/tokens.
	 */
	~LitNode(void);

private:
	// "Required" items- checked for correctness on creation.
	Type_t                m_cType;
	std::deque<LexToken*> m_tkns;
};

#endif /* DS_LITNODE_H_ */
