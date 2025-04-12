/*
 * StepReadFile.cpp: Step to parse file into program as AST.
 */

#include "Common/Device/File.h"
#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Common/Util/InfoUtil.h"
#include "Domain/RetCode_e.h"
#include "State/SubStepLexFile.h"
#include "State/SubStepParseTkns.h"

#include "State/StepReadFile.h"

using namespace std;

//==============================================================================
// Helper function to open a file for reading (with debug/error handling).
static void StepReadFiles_openReadFile(DataModel_t& model,
		                               string const& fname) {
	// (Stay informative.)
	Print::inst().log(LOG_DEBUG, string("Reading '") + fname + "'...");

	// Actually open file.
	RetErr_e retErr = File::inst().open(fname, FILE_OP_READ);

	// Record error on occurrence.
	if (retErr) {
		Print::inst().log(LOG_ERROR, string("Can't read '") + fname + "'");
		InfoUtil_recordError(model.m_summary, RET_NO_READ);
	}
}

//==============================================================================
// Parse source file (and associated include files) into ASTs.
void StepReadFiles_execute(DataModel_t& model, std::string const& fname) {
	// Open source file.
	StepReadFiles_openReadFile(model, fname);

	// Lex into tokens.
	queue<LexToken*> tkns = {};
	if (model.m_summary.m_numErrs == 0) {
		SubStepLexFile_execute(model, tkns);
	}

	// Parse into AST.
	if (model.m_summary.m_numErrs == 0) {
		model.m_ast = SubStepParseTkns_execute(model, tkns);
	}

	// Close source file.
	File::inst().close();

	// Exit program (vs step) as applicable.
	if (model.m_summary.m_numErrs > 0) {
		Terminate::inst().summary(model.m_summary);
	}
}
