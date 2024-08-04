/*
 * ReadFilesStep.cpp
 *
 * "Step of assembly process focused on reading each input file"
 */

#include "DeviceLayer/FileReader.h"
#include "DeviceLayer/LexScanner.h"
#include "DeviceLayer/Parser.h"
#include "DeviceLayer/Printer.h"
#include "DeviceLayer/Terminate.h"
#include "Utils/ErrorUtils.h"

#include "AppLayer/ReadFilesStep.h"

using namespace std;

//==============================================================================
// Reads input files specified in model. Adds parsed/checked items to model.
void ReadFilesStep_readFiles(DataModel_t* model) {
	// Read each file- scanning, parsing, and analyzing each as its own unit.
	for (string inFile : model->m_inFiles) {
		// (Inform info users).
		string infoStr = string("Reading \"") + inFile + "\"...";
		Printer::getInst()->log(LOG_INFO, infoStr);

		// Open the file for reading.
		FileReader* readFile = new FileReader(model, inFile);

		// Scan file for tokens (as able to).
		queue<ScanToken_t*>* scanTkns = nullptr; // failed till proven success
		if (readFile->isReady()) {
			scanTkns = LexScanner::getInst()->scanFile(model, readFile);
		}

		// Parse tokens into build items (as able to).
		vector<IBuildItem*>* buildItems = nullptr;
		if (scanTkns != nullptr) {
			buildItems = Parser::getInst()->parseTokens(model, scanTkns);
		}

		// Analyze tokens locally (as able to).
		if (buildItems != nullptr) {
			// Analyze each item.
			for (IBuildItem* item : *buildItems) {
				item->doLocalAnalysis(model);
			}

			// Report any un-paired labels.
			for (string label : model->m_openLabels) {
				string errStr = "Label \"" +
						        label +
								"\" does not pair to anything";
				Printer::getInst()->log(LOG_ERR, inFile, errStr);
				ErrorUtils_includeReason(model, REASON_NO_PAIRING);
			}
		}

		// "File" parsed and analyzed- add to cumulative program.
		if (buildItems != nullptr) {
			// Add  analyzed file items.
			for (IBuildItem* item : *buildItems) {
				model->m_items.push_back(item);
			}
		}

		// (Clean up memory). // TODO- STLs should be safe/good to "de-ptr-ize"
		delete scanTkns;
		delete buildItems;

		// Handle "outputs" of file unit.
		delete readFile;
		Printer::getInst()->printLog();
		if (model->m_firstReason != REASON_SUCCESS) { // failure occurred
			// Program failed- terminate for listed reason.
			Printer::getInst()->printSummary(*model);
			Printer::getInst()->printLog();
			Terminate::getInst()->exit(model->m_firstReason);
		}
	}
}
