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
// Executes process to analyze each input file in the model, saving the results.
void StepReadFiles_execute(DataModel_t& model) {
	// (Inform User.)
	Print::inst().log(LOG_INFO, "=Read Files=");

	// Read/analyze each file- stopping if a file had errors.
	RetErr_e retErr = RET_ERR_NONE; // INNOCENT till guilty
	for (string fname : model.m_files) {
		// Open the file (for reading).
		retErr = File::inst().open(fname, FILE_OP_READ);
		Print::inst().log(LOG_INFO, string("Reading '") + fname + "'...");
		if (retErr) {
			Print::inst().log(LOG_ERROR, string("Can't read '") + fname + "'");
			ModelUtil_recordError(model, RET_NO_READ);
		}

		// Lex the file- creating an ordered list of tokens.
		TokenList_t fileTkns;
		if (retErr == RET_ERR_NONE) {
			retErr = SubStepLexFile_execute(model, fileTkns);
		}

		// Finished with raw file's contents.
		File::inst().close();

		// Now parse the tokens into a localized "program" for analysis.
		NodeList_t fileNodes;
		if (retErr == RET_ERR_NONE) {
			retErr = SubStepParseTkns_execute(model, fileTkns, fileNodes);
		}

		// Analyze nodes locally- accumulating them into the model (in order).
		if (retErr == RET_ERR_NONE) {
			// Analyze each node in file order.
			for (AAsmNode* node : fileNodes) {
				// (Sanity check node.)
				if (node == nullptr) {Terminate_assert("Analyzed null node");}

				// Analyze node.
				node->doLocalAnalysis(model);

				// Save (in order) to model.
				model.m_nodes.push_back(move(node)); // (pass ownership)
			}

			// Update local record based on analysis.
			if (model.m_numErrs > 0) {retErr = RET_ERR_ERROR;}
		}

		// Stop reading files if errors were found.
		if (retErr) {break;}
	}

	// Terminate program (with summary) if errors were found.
	if (retErr) {Terminate_summary(model);}
}
