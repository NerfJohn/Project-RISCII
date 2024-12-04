/*
 * StepReadFiles.cpp: State/step to parse and analyze the input files.
 */

#include "Device/File.h"
#include "Device/Print.h"
#include "Device/SymTable.h"
#include "Device/Terminate.h"
#include "Ds/AAsmNode.h"
#include "State/SubStepLexFile.h"
#include "State/SubStepParseTkns.h"
#include "Util/ModelUtil.h"

#include "State/StepReadFiles.h"

using namespace std;

//==============================================================================
// Helper function to open a file for reading (with info/error handling).
static void StepReadFiles_openReadFile(DataModel_t& model,
		                               string const& fname) {
	// (Stay informative.)
	Print::inst().log(LOG_INFO, string("Reading '") + fname + "'...");

	// Actually open file.
	RetErr_e retErr = File::inst().open(fname, FILE_OP_READ);

	// Record error on occurrence.
	if (retErr) {
		Print::inst().log(LOG_ERROR, string("Can't read '") + fname + "'");
		ModelUtil_recordError(model, RET_NO_READ);
	}
}

//==============================================================================
// Helper function to add nodes/symbols to the model's overall program data.
static void StepReadFiles_addElements(DataModel_t& model,
		                              std::vector<AAsmNode*>& nodes,
									  SymTable& table) {
	// Nodes can simply be added- we've handled what we needed to.
	for (AAsmNode* node : nodes) {model.m_nodes.push_back(node);}

	// Find global symbols to add them to model.
	vector<Symbol_t*> symbols;
	table.asList(symbols);
	for (Symbol_t* sym : symbols) {
		// Only add "global" labels.
		IF_NULL(sym, "add() null symbol to global table");
		if (sym->m_isGlobal &&
			(model.m_gSyms.addSym(sym->m_name, sym) == RET_ERR_ERROR)) {
			// Already defined- get other definition log.
			Symbol_t* glbSym = nullptr;
			model.m_gSyms.getSym(sym->m_name, glbSym);
			IF_NULL(sym, "add() got null symbol");

			// Log error.
			string errStr = string("Global re-def '") +
					        sym->m_name               +
							"' (first at "            +
							glbSym->m_file            +
							"/"                       +
							to_string(glbSym->m_line) +
							")";
			Print::inst().log(LOG_ERROR,sym->m_file, sym->m_line , errStr);
			ModelUtil_recordError(model, RET_G_REDEF);
		}
	}
}

//==============================================================================
// Executes process to analyze each input file in the model, saving the results.
void StepReadFiles_execute(DataModel_t& model) {
	// (Inform User.)
	Print::inst().log(LOG_INFO, "=Read Files=");

	// Read/analyze each file- stopping for error-ridden file.
	for (string fname : model.m_files) {
		// Open file (for reading).
		StepReadFiles_openReadFile(model, fname);

		// Lex file- creating (owned) list of tokens.
		queue<ItemToken*> fileTkns;
		if (model.m_numErrs == 0) {
			SubStepLexFile_execute(model, fileTkns);
		}

		// Finished with now token-ized file.
		File::inst().close();

		// Parse tokens- creating (owned) list of file-local nodes.
		vector<AAsmNode*> fileNodes;
		if (model.m_numErrs == 0) {
			SubStepParseTkns_execute(model, fileTkns, fileNodes);
		}

		// With file now parsed, perform localized checks/analysis/saving.
		if (model.m_numErrs == 0) {
			// FIRST, analyze each node- checking args/local symbols.
			SymTable localSyms;
			for (AAsmNode* node : fileNodes) {
				IF_NULL(node, "Analyzed null node");
				node->localAnalyze(model, localSyms);
			}

			// NEXT, perform local linking/linkage handling.
			for (AAsmNode* node : fileNodes) {
				IF_NULL(node, "Locally linked null node");
				node->localLink(model, localSyms);
			}

			// LAST, add nodes/symbols to overall program (in model).
			StepReadFiles_addElements(model, fileNodes, localSyms);
		}

		// Stop reading files if errors were found.
		if (model.m_numErrs > 0) {break;}
	}

	// Terminate program (with summary) if errors were found.
	if (model.m_numErrs > 0) {Terminate_summary(model);}
}
