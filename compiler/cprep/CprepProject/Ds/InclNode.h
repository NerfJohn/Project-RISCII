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
	 * @brief Locate included files, adding them to the model for processing.
	 *
	 * Implements basic "quotation" inclusion logic. Includes scheduled for
	 * parsing only if haven't been already. Checks all includes- regardless
	 * of pre-processing conditions.
	 *
	 * @param model data model of the entire program
	 */
	void findIncludes(DataModel_t& model);

	// TODO
	void checkDefines(DataModel_t&model);

	// TODO
	void writeText(DataModel_t& model);

	/*
	 * @brief Std destructor- deletes underlying nodes/tokens.
	 */
	~InclNode(void);

private:
	// "Required" items- checked for correctness on creation.
	std::string m_reqFname;

	// Items determined while processing.
	std::string m_finFname;

	// Helper function to check if file exists.
	bool fileExists(std::string const& fname);
};

#endif /* DS_INCLNODE_H_ */
