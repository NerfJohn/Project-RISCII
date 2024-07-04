/*
 * ParseCliStep.cpp
 *
 * "Step of assembly process focused on parsing the command line inputs"
 */

#include "AppLayer/ParseCliStep.h"

using namespace std;

//==============================================================================
// Parses options and filename inputs into the data model.
void ParseCliStep_parseCli(int argc, char* argv[], DataModel_t* model) {
	// Iterate through arguments- adding input filenames to model.
	for (int i = 1; i < argc; i++) { // skip exe name (ie index 0).
		// C++ stringify the filename.
		string inFilename = argv[i];

		// Append it to data model's records.
		model->m_inFiles.push_back(inFilename);
	}
}
