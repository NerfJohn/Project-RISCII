/*
 * StepReadFiles.cpp: State/step to parse and analyze the input files.
 */

#include "Device/File.h"
#include "Device/Print.h"
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
			// Analyze each node- checking args/local symbols.
			// TODO- create local symbol table
			for (AAsmNode* node : fileNodes) {
				if (node == nullptr) {Terminate_assert("Analyzed null node");}
				node->doLocalAnalysis(model);
			}

			// TODO- perform local linking/linkage handling.

			// Add nodes to cumulative program.
			for (AAsmNode* node : fileNodes) {
				// (Sanity check.)
				if (node == nullptr) {Terminate_assert("Added null node");}
				model.m_nodes.push_back(move(node)); // (pass ownership)
			}
		}

		// Stop reading files if errors were found.
		if (model.m_numErrs > 0) {break;}
	}

	// Terminate program (with summary) if errors were found.
	if (model.m_numErrs > 0) {Terminate_summary(model);}
}
