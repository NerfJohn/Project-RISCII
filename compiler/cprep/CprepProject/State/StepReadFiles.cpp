/*
 * StepReadFiles.cpp: State/step to parse and analyze the input/include files.
 */

#include "Device/File.h"
#include "Device/Print.h"
#include "Device/Terminate.h"
#include "State/SubStepLexFile.h"
#include "State/SubStepParseTkns.h"
#include "Util/ModelUtil.h"

#include "State/StepReadFiles.h"

using namespace std;

//==============================================================================
// Helper function to open a file for reading (with info/error handling).
static void StepReadFiles_openReadFile(DataModel_t& model,
		                               string const& fname) {
	// (Provide debug data- hard read vs re-reference.)
	Print::inst().log(LOG_DEBUG, string("Reading '") + fname + "'...");

	// Actually open file.
	RetErr_e retErr = File::inst().open(fname, FILE_OP_READ);

	// Record error on occurrence.
	if (retErr) {
		Print::inst().log(LOG_ERROR, string("Can't read '") + fname + "'");
		ModelUtil_recordError(model, RET_NO_READ);
	}
}

//==============================================================================
// Executes process to analyze an input + include files, saving the results.
void StepReadFiles_execute(DataModel_t& model, std::string const& fname) {
	// TODO- redo for "while any files to read".

	// (Inform User.)
	Print::inst().log(LOG_INFO, "Checking '" + fname + "'...");

	// Open file (for reading).
	StepReadFiles_openReadFile(model, fname);

	// Lex file- creating (owned) list of tokens.
	queue<ItemToken*> fileTkns;
	if (model.m_numErrs == 0) {
		SubStepLexFile_execute(model, fileTkns);
	}

	// Finished with now token-ized file.
	File::inst().close();

	// Parse tokens into AST (for this file).
	AAstNode* root = nullptr;
	if (model.m_numErrs == 0) {
		SubStepParseTkns_execute(model, fileTkns, root);
	}

	// Terminate program (with summary) if errors were found.
	if (model.m_numErrs > 0) {Terminate_summary(model);}
}
