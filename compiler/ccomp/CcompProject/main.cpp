/*
 * main.cpp: Head of the program/application.
 */

#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Common/Util/Msg.h"
#include "Domain/DataModel_t.h"
#include "State/StepLexFile.h"
#include "State/StepParseCli.h"
#include "State/StepParseFile.h"
#include "Util/AppUtil.h"

using namespace std;

//==============================================================================
// Start/main process of the program.
int main(int argc, char* argv[]) {
	// Init devices.
	Terminate_setBugCode(RET_ASSERT);
	Print_setLogLevel(LOG_WARNING);   // default = log errors/warnings
	Print_setLogName("ccomp.exe");    // Set to name of program

	// Init program's data.
	DataModel_t model = {
		// General Summary/Progress.
		.m_numErrs  = 0,
		.m_numWarns = 0,
		.m_exitCode = RET_SUCCESS,               // start optimistic

		// Parsed Cli Results.
		.m_files = {},
		.m_doOpt = false,                        // default = no optimization

		// AST Processing Artifacts.
		.m_tkns = {},
		.m_ast  = Ptr<IAstNode>(nullptr)
	};

	// Collect inputs.
	bool cliExit = StepParseCli_execute(model, argc, argv);

	// Compile each file.
	if (false == cliExit) {
		Print_log(LOG_INFO, "=Process=");
		for (string file : model.m_files) {
			// Prep/cleanup.
			model.m_tkns = {};

			// Compile.
			Print_log(LOG_INFO, Msg() + "Compiling '" + file + "'...");
			if (APP_OK){StepLexFile_execute(model, file);}
			if (APP_OK){StepParseFile_execute(model);}

			// Breakout for errors.
			if (APP_OK == false) {break;}
		}
	}

	// Exit with summary (nix cli- no log level resolution).
	if (cliExit) {Terminate_silent(model.m_exitCode);}
	else         {AppUtil_exitSummary(model);}
	return RET_SUCCESS; // Not reached- appeasing g++
}
