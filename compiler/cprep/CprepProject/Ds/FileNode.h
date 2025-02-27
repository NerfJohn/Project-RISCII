/*
 * FileNode.h: Node representing one pre-processed/parsed file.
 */

#ifndef DS_FILENODE_H_
#define DS_FILENODE_H_

#include <stack>
#include "Ds/IAstNode.h"

/*
 * @brief Represents file parsed into pre-processor nodes.
 *
 * File may be source or include file.
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
	 * @brief Std destructor- deletes underlying nodes/tokens.
	 */
	~FileNode(void);

private:
	// "Required" items- checked for correctness on creation.
	std::deque<IAstNode*> m_reqNodes;
};

#endif /* DS_FILENODE_H_ */
