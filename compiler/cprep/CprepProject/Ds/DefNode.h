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
	 * @brief Verifies definitions/conditions are correct.
	 *
	 * Affects data stored in ASTs (src and included). Re-calling with different
	 * model info will overwrite previous progress.
	 *
	 * @param model data model of the entire program
	 */
	void checkDefines(DataModel_t&model);

	/*
	 * @brief Std destructor- deletes underlying nodes/tokens.
	 */
	~DefNode(void);

private:
	// "Required" items- checked for correctness on creation.
	std::string m_reqDef;

	// Items filled in while processing.
	std::string m_defSym; // "Symbol" for definition- namely for displaying
};

#endif /* DS_DEFNODE_H_ */
