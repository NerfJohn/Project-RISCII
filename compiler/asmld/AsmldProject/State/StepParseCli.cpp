/*
 * StepParseCli.cpp: State/step to process command line inputs into data model.
 */

#include "Device/GetOpt.h"
#include "Device/Print.h"

#include "State/StepParseCli.h"

using namespace std;

//==============================================================================
// Executes process to check/parse arguments from command line into data model.
void StepParseCli_execute(DataModel_t& model,
		                  int const argc,
						  char const* const argv[]) {
	// Get cli arguments in parse-able means.
	GetOpt args(argc, argv);
	args.getOpt();           // first arg is program file- discard

	// Parse each cli argument.
	while(args.getOpt()) {
		// TODO- for now, only record/check files.
		if (args.m_type == CLI_TYPE_FILE) {
			model.m_files.push_back(args.m_value);
		}
	}

	// (Book-keep.)
	string numFiles = string("\t# files: ") + to_string(model.m_files.size());
	Print::inst().log(LOG_INFO, "=Cli Summary=");
	Print::inst().log(LOG_INFO, numFiles);
}

