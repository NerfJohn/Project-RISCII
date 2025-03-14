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
	 * @brief Verifies definitions/conditions are correct.
	 *
	 * Affects data stored in ASTs (src and included). Re-calling with different
	 * model info will overwrite previous progress.
	 *
	 * @param model data model of the entire program
	 */
	void checkDefines(DataModel_t&model);

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
	~FileNode(void);

private:
	// "Required" items- checked for correctness on creation.
	std::deque<IAstNode*> m_reqNodes;
};

#endif /* DS_FILENODE_H_ */
