/*
 * DataModel_t.h: Critical data passed between states/steps of the program.
 */

#ifndef DOMAIN_DATAMODEL_T_H_
#define DOMAIN_DATAMODEL_T_H_

#include <cstdint>
#include <queue>
#include <string>
#include <vector>
#include "Common/Device/SymTable.h"
#include "Common/Ds/LexToken.h"
#include "Common/Util/Ptr.h"
#include "Domain/RetCode_e.h"
#include "Domain/Sym_t.h"
#include "Ds/IAstNode.h"

/*
 * @brief Data passed in-between states/pieces of the entire program.
 *
 * "Model" of the program with respect to data- any info shared between larger
 * parts of the program is saved and shared via this struct. It is intended for
 * the program to only ever have one, shared instance of this type.
 */
typedef struct datamodel{
	// General Summary/Progress.
	uint32_t  m_numErrs;                         // # of errors found
	uint32_t  m_numWarns;                        // # of warnings found
	RetCode_e m_exitCode;                        // exit code to exit with

	// Parsed Cli Results.
	std::vector<std::string> m_files;            // input files
	bool                     m_doOpt;            // optimize option

	// AST Processing Artifacts.
	std::queue<Ptr<LexToken>> m_tkns;            // lex tokens
	Ptr<IAstNode>             m_ast;             // current AST
	SymTable<Ptr<Sym_t>>      m_syms;            // current sym table
	Ptr<Sym_t>                m_fSym;            // sym of current function
	uint32_t                  m_symCnt;          // # of symbols created


} DataModel_t;

#endif /* DOMAIN_DATAMODEL_T_H_ */
