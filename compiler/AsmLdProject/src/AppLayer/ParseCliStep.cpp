/*
 * ParseCliStep.cpp
 *
 * "Step of assembly process focused on parsing the command line inputs"
 */

#include "DeviceLayer/Printer.h"
#include "DeviceLayer/Terminate.h"
#include "Utils/CliUtils.h"
#include "Utils/ErrorUtils.h"
#include "Utils/StringUtils.h"

#include "AppLayer/ParseCliStep.h"

using namespace std;

//==============================================================================

// Definitions for output file configuration.
#define FIRST_INPUT_IDX (0)
#define OUT_FILE_TYPE   string(".bin")

// Local type for "state machine" of parsing CLI.
typedef enum {
	CLI_STATE_FILE, // typical, parsing files state
	CLI_STATE_ARG   // parsing flag argument state
} CliState_e;

//==============================================================================
// Parses CLI input into data model, implementing select flags immediately.
void ParseCliStep_parseCli(int argc, char* argv[], DataModel_t& model) {
	// Prepare loop variants for CLI parsing.
	CliState_e cliState = CLI_STATE_FILE;
	string flagStr = "";

	// Iterate through arguments- parsing each in kind.
	for (int i = 1; i < argc; i++) { // skip exe name (ie index 0).
		// Ensure token string exists.
		if ((argv == 0) || (argv[i] == 0)) {
			// Design error (should come directly from main). Assert!
			string asrtStr = "CLI token array not arranged as expected";
			Printer::getInst()->printAssert(asrtStr);
			Terminate::getInst()->exit(REASON_ASSERT);
		}

		// C++ stringify the filename.
		string argStr = argv[i];

		// Parse depending on the state.
		if (cliState == CLI_STATE_ARG) {
			// Argument is flag argument- save to model.
			RetErr_e retErr = CliUtils_saveToModel(model, flagStr, argStr);
			if (retErr) {
				// (Likely) poor argument for flag.
				string errStr = string("Argument \"") +
						        argStr +
								"\" invalid for flag \"" +
								flagStr +
								"\"";
				Printer::getInst()->log(LOG_ERR, errStr);
				ErrorUtils_includeReason(&model, REASON_FLAG_ARG);
			}

			// Return to typical parsing.
			cliState = CLI_STATE_FILE;
		}
		else { // cliState == CLI_STATE_FILE
			// Determine type of argument given.
			CliType_e argType = CliUtils_asCliType(argStr);

			// Handle type accordingly.
			switch (argType) {
				case CLI_BAD_FLAG:
				{
					// Unrecognized flag- report error.
					string errStr = string("Invalid flag \"") + argStr + "\"";
					Printer::getInst()->log(LOG_ERR, errStr);
					ErrorUtils_includeReason(&model, REASON_INVALID_FLAG);
					break;
				}
				case CLI_LONE_FLAG:
				{
					// Lone flag- immediately save to model.
					RetErr_e retErr = CliUtils_saveToModel(model, argStr);
					if (retErr) {
						// Shouldn't have been a CLI_LONE_FLAG. assert!
						string assertStr = "couldn't save lone flag";
						Printer::getInst()->printAssert(assertStr);
						Terminate::getInst()->exit(REASON_ASSERT);
					}
					break;
				}
				case CLI_ARG_FLAG:
				{
					// Flag with argument- must resolve with next argument.
					flagStr = argStr;
					cliState = CLI_STATE_ARG;
					break;
				}
				case CLI_FILE:
				default:
					// Argument is a filename- save as such.
					model.m_inFiles.push_back(argStr);
			}
		}
	}

	// Ensure no arg flags were left without an arg.
	if (cliState == CLI_STATE_ARG) {
		string errStr = string("Flag \"") + flagStr + "\" requires an argument";
		Printer::getInst()->log(LOG_ERR, errStr);
		ErrorUtils_includeReason(&model, REASON_NO_FLAG_ARG);
	}

	// Ensure at least one input file is given to work with.
	if (model.m_inFiles.size() == 0) {
		string errStr = "No input files provided";
		Printer::getInst()->log(LOG_ERR, errStr);
		ErrorUtils_includeReason(&model, REASON_NO_INPUTS);
	}

	// Ensure an output filename exists- base off first input file as needed.
	if ((model.m_outFile.size() == 0) &&
		(model.m_inFiles.size() > 0)) {
		string baseFilename = model.m_inFiles.at(FIRST_INPUT_IDX);
		model.m_outFile = StringUtils_removeFileType(baseFilename)
				           + OUT_FILE_TYPE;
	}

	// Handle CLI log requests by setting the printer.
	Printer::getInst()->setLogLevel(model.m_logLevel);

	// Handle CLI help requests by printing (followed by exiting).
	if (model.m_helpFlag) {
		Printer::getInst()->printHelp();
		Terminate::getInst()->exit(REASON_SUCCESS);
	}

	// (Inform debugging users).
	string dbgStr = "Options = {outFile = \"" + model.m_outFile + "\"}";
	Printer::getInst()->log(LOG_DEBUG, dbgStr);

	// Handle additional "outputs" of the step.
	Printer::getInst()->printLog();
	if (model.m_firstReason != REASON_SUCCESS) { // failure occurred
		// Program failed- terminate for listed reason.
		Printer::getInst()->printSummary(model);
		Printer::getInst()->printLog();
		Terminate::getInst()->exit(model.m_firstReason);
	}
}
