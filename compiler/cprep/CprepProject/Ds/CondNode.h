/*
 * CondNode.h: Node representing conditionally included section of file.
 */

#ifndef DS_CONDNODE_H_
#define DS_CONDNODE_H_

#include <stack>
#include "Ds/IAstNode.h"

/*
 * @brief Represents section of file conditionally included/copied.
 *
 * Condition based on existence (or lack thereod) of preprocessor variable.
 */
class CondNode: public IAstNode {
public:
	/*
	 * @brief Constructor- creates node from action stack items.
	 *
	 * Assumes action stack has been grammar checked/parsed for correctness.
	 * Intended to be used with Common/Device/Parser.
	 *
	 * @param actStack action stack with node's build items
	 */
	CondNode(std::stack<IBuildItem*>& actStack);

	/*
	 * @brief Locate included files, adding them to the model for processing.
	 *
	 * Implements basic "quotation" inclusion logic. Includes scheduled for
	 * parsing only if haven't been already. Checks all includes- regardless
	 * of pre-processing conditions.
	 *
	 * @param model data model of the entire program
	 */
	void findIncludes(DataModel_t& model);

	/*
	 * @brief Std destructor- deletes underlying nodes/tokens.
	 */
	~CondNode(void);

private:
	// "Required" items- checked for correctness on creation.
	bool                  m_reqIfDefined;
	std::string           m_reqDef;
	std::deque<IAstNode*> m_reqNodes;
};

#endif /* DS_CONDNODE_H_ */
