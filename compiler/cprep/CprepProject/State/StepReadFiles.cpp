/*
 * StepReadFiles.cpp: Step to process source file + associated include files.
 */

#include "Common/Device/File.h"
#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Common/Util/InfoUtil.h"
#include "Domain/RetCode_e.h"
#include "State/SubStepLexFile.h"
#include "State/SubStepParseTkns.h"

#include "State/StepReadFiles.h"

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
void StepReadFiles_execute(DataModel_t& model, std::string const& srcName) {
	// (Stay informative.)
	Print::inst().log(LOG_INFO, string("Processing '") + srcName + "'...");

	// Init execution critical data.
	model.m_srcAst = nullptr;
	while(model.m_incPaths.size() > 0) {model.m_incPaths.pop();}
	model.m_incPaths.push(srcName);

	// Recursively parse files- finding new ones from includes.
	while(model.m_incPaths.size() > 0) {
		// Open file.
		string fname = model.m_incPaths.front();
		StepReadFiles_openReadFile(model, fname);
		model.m_incPaths.pop();                   // read started- bookkeep

		// Create AST.
		queue<LexToken*> tkns = {};
		IAstNode*        ast  = nullptr;
		if (model.m_summary.m_numErrs == 0) {
			SubStepLexFile_execute(model, tkns);
		}
		if (model.m_summary.m_numErrs == 0) {
			ast = SubStepParseTkns_execute(model, tkns);
		}

		// Close file.
		File::inst().close();

		// Find/queue new includes.
		if (model.m_summary.m_numErrs == 0) {
			IF_NULL(ast, "readfiles() null ast");
			ast->findIncludes(model);
		}

		// Save AST appropriately.
		if (model.m_srcAst == nullptr) {model.m_srcAst = ast;}
		else                           {model.m_iAsts.addLocal(fname, ast);}

		// Errors? break out early.
		if (model.m_summary.m_numErrs > 0) {break;}
	}

	// Exit program (vs step) as applicable.
	if (model.m_summary.m_numErrs > 0) {
		Terminate::inst().summary(model.m_summary);
	}
}
