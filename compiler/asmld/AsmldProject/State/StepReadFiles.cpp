/*
 * StepReadFiles.cpp: State/step to parse and analyze the input files
 */

#include "Device/File.h"
#include "Device/Print.h"
#include "Device/Terminate.h"
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

		// Finished with raw file's contents.
		File::inst().close();

		// Stop reading files if errors were found.
		if (retErr) {break;}
	}

	// Terminate program (with summary) if errors were found.
	if (retErr) {Terminate_summary(model);}
}
