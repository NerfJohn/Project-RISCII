/*
 * ParseCliStep.cpp
 *
 * "Step of assembly process focused on parsing the command line inputs"
 */

#include "DeviceLayer/Printer.h"
#include "DeviceLayer/Terminate.h"
#include "Utils/ErrorUtils.h"
#include "Utils/StringUtils.h"

#include "AppLayer/ParseCliStep.h"

using namespace std;

//==============================================================================

// Definitions for input requirements.
#define INPUT_FILES_MIN (1)

// Definitions for output file configuration.
#define FIRST_INPUT_IDX (0)
#define OUT_FILE_TYPE   string(".bin")

//==============================================================================
// Parses options and filename inputs into the data model.
void ParseCliStep_parseCli(int argc, char* argv[], DataModel_t* model) {
	// Iterate through arguments- adding input filenames to model.
	for (int i = 1; i < argc; i++) { // skip exe name (ie index 0).
		// (Inform debugging users).
		string dbgStr = string("Parsing \"") + argv[i] + "\"";
		Printer::getInst()->log(LOG_DEBUG, dbgStr);

		// C++ stringify the filename.
		string inFilename = argv[i];

		// Append it to data model's records.
		model->m_inFiles.push_back(inFilename);
	}

	// Ensure at least one input file is provided.
	if (model->m_inFiles.size() < INPUT_FILES_MIN) {
		string errStr = "No input files provided";
		Printer::getInst()->log(LOG_ERR, errStr);
		ErrorUtils_includeReason(model, REASON_NO_INPUTS);
	}

	// Set the output filename- based on first input file (as able).
	if (model->m_inFiles.size() >= INPUT_FILES_MIN) {
		string baseFilename = model->m_inFiles.at(FIRST_INPUT_IDX);
		model->m_outFile = StringUtils_removeFileType(baseFilename)
				           + OUT_FILE_TYPE;
	}

	// Handle additional "outputs" of the step.
	Printer::getInst()->printLog();
	if (model->m_firstReason != REASON_SUCCESS) { // failure occurred
		// Program failed- terminate for listed reason.
		Printer::getInst()->printSummary(*model);
		Printer::getInst()->printLog();
		Terminate::getInst()->exit(model->m_firstReason);
	}

}
