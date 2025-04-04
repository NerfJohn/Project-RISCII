/*
 * main.cpp: Head of the program/application.
 */

#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Domain/DataModel_t.h"
#include "Domain/RetCode_e.h"
#include "State/StepParseCli.h"

// TODO
#include "Common/Device/File.h"
#include "Common/Util/StrUtil.h"
#include "State/SubStepLexFile.h"

using namespace std;

//==============================================================================
// Start/main process of the program.
int main(int argc, char* argv[]) {
	// Init devices.
	Print::inst().setLogName("ccomp.exe");
	Print::inst().setLogLevel(LOG_WARNING);
	Terminate::inst().setAssertCode(RET_ASSERT);

	// Init data model.
	PrgmInfo_t runSummary = {
		.m_numWarns = 0,                         // no initial warnings
		.m_numErrs  = 0,                         // no initial errors
		.m_retCode  = RET_SUCCESS                // good so far
	};
	DataModel_t prgmData = {
		// General Summary/Progress.
		.m_summary = runSummary,                 // (see above)

		// Parsed Cli Data.
		.m_files = {},                           // no initial files
		.m_doOpt = false                         // default- no optimization
	};

	// Parse program's cli command/call.
	StepParseCli_execute(prgmData, argc, argv);

	// TODO- "translate" files.
	for (string file : prgmData.m_files) {
		// Read in file.
		string contents = "";
		File::inst().open(file, FILE_OP_READ);
		//while(File::inst().peek() != 0xFF) {contents += File::inst().pop();}
		queue<LexToken*> tkns;
		SubStepLexFile_execute(prgmData, tkns);
		File::inst().close();

		while(tkns.size()) {Print::inst().cli(to_string(tkns.front()->m_type)); tkns.pop();}

		/*
		// Get new filename.
		string fname = file;
		StrUtil_rmFtype(fname);
		fname += ".s";

		// Populate new file.
		File::inst().open(fname, FILE_OP_WRITE);
		File::inst().write(contents);
		File::inst().close();
		*/
	}


	// End program with summary of run instance.
	Terminate::inst().summary(prgmData.m_summary);
	return RET_SUCCESS; /* never reached, but appeal to g++ */
}
