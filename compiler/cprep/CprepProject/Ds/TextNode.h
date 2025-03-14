/*
 * TextNode.h: Node representing non-pre-processor related source text.
 */

#ifndef DS_TEXTNODE_H_
#define DS_TEXTNODE_H_

#include <stack>
#include "Ds/IAstNode.h"

/*
 * @brief Represents text from file unrelated to pre-processing.
 *
 * Aggregates tokens into single string.
 */
class TextNode: public IAstNode {
public:
	/*
	 * @brief Constructor- creates node from action stack items.
	 *
	 * Assumes action stack has been grammar checked/parsed for correctness.
	 * Intended to be used with Common/Device/Parser.
	 *
	 * @param actStack action stack with node's build items
	 */
	TextNode(std::stack<IBuildItem*>& actStack);

	/*
	 * @brief Writes contents (as applicable) to already opened write file.
	 *
	 * Assumes write file opened with Common/Device/File. Takes paths based on
	 * progress determined by calling checkDefines().
	 *
	 * @param model data model of the entire program
	 */
	void writeText(DataModel_t& model);

	/*
	 * @brief Std destructor- deletes underlying nodes/tokens.
	 */
	~TextNode(void);

private:
	// "Required" items- checked for correctness on creation.
	std::string m_reqText;
};

#endif /* DS_TEXTNODE_H_ */
