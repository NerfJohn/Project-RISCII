/*
 * DataModel_t.h: Critical data passed between states/steps of the program.
 */

#ifndef DOMAIN_DATAMODEL_T_H_
#define DOMAIN_DATAMODEL_T_H_

#include <string>
#include <vector>
#include <queue>
#include "Common/Device/SymTable.h"
#include "Common/Domain/PrgmInfo_t.h"

// #include "Ds/IAstNode.h" without the recursive dependency.
class IAstNode;

/*
 * @brief Data passed in-between states/pieces of the entire program.
 *
 * "Model" of the program with respect to data- any info shared between larger
 * parts of the program is saved and shared via this struct. It is intended for
 * the program to only ever have one, shared instance of this type.
 */
typedef struct {
	// General Summary/Progress.
	PrgmInfo_t m_summary;                        // summary of run instance

	// Parsed Cli Data.
	std::vector<std::string> m_files;            // input source files
	std::vector<std::string> m_iDirs;            // include directories

	// Processed data.
	IAstNode*               m_srcAst;            // AST of source file
	SymTable                m_iAsts;             // include file ASTs
	std::queue<std::string> m_incPaths;          // include file to parse
} DataModel_t;

#endif /* DOMAIN_DATAMODEL_T_H_ */
