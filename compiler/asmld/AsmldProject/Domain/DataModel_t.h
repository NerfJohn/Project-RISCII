/*
 * DataModel_t.h: Critical data passed between states/steps of the program.
 */

#ifndef DOMAIN_DATAMODEL_T_H_
#define DOMAIN_DATAMODEL_T_H_

#include <cstdint>
#include <string>
#include <vector>
#include "Device/SymTable.h"
#include "Domain/RetCode_e.h"

// Forward declare to avoid recursive includes.
class AAsmNode;

/*
 * Data passed in-between states/pieces of the entire program.
 *
 * "Model" of the program with respect to data- any info shared between larger
 * parts of the program is saved and shared via this struct. It is intended for
 * the program to only ever have one, shared instance of this type.
 */
typedef struct {
	// General Summary/Progress.
	uint32_t                 m_numWarns;         // number of found warnings
	uint32_t                 m_numErrs;          // number of found errors
	RetCode_e                m_retCode;          // code to return on exit

	// Parsed Cli Data.
	std::vector<std::string> m_files;            // input source files
	std::string              m_outFile;          // name of created file
	bool                     m_doDbg;            // enables adding debug symbols

	// Parsed/analyzed program.
	std::vector<AAsmNode*>   m_nodes;            // nodes creating the program
	SymTable                 m_gSyms;            // global symbols
	AAsmNode*                m_start;            // starting node of program
	bool                     m_hasData;          // has natural data section
	std::vector<Symbol_t*>   m_openLabels;       // unpaired labels

	// Binary image results.
	uint32_t                 m_textSize;         // number of text words
	uint32_t                 m_dataSize;         // number of data words
	uint32_t                 m_bssSize;          // number of bss words
	std::vector<uint16_t>    m_textVals;         // ordered text section values
	std::vector<uint16_t>    m_dataVals;         // ordered data section values
} DataModel_t;

#endif /* DOMAIN_DATAMODEL_T_H_ */
