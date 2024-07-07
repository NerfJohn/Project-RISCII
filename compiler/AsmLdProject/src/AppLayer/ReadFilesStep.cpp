/*
 * ReadFilesStep.cpp
 *
 * "Step of assembly process focused on reading each input file"
 */

#include "DeviceLayer/FileReader.h"
#include "DeviceLayer/LexScanner.h"
#include "DeviceLayer/Printer.h"
#include "DeviceLayer/Terminate.h"
#include "Utils/ErrorUtils.h"

#include "AppLayer/ReadFilesStep.h"

using namespace std;

//==============================================================================
// TODO- desc.
void ReadFilesStep_readFiles(DataModel_t* model) {
	// Read each file- scanning, parsing, and analyzing each as its own unit.
	for (string inFile : model->m_inFiles) {
		// Open the file for reading.
		FileReader* readFile = new FileReader(model, inFile);

		// Scan file for tokens (as able to).
		queue<ScanToken_t>* scanTkns = nullptr; // failed till proven success
		if (readFile->isReady()) {
			scanTkns = LexScanner::getInst()->scanFile(model, readFile);
		}

		// Handle "outputs" of file unit.
		delete readFile;
		Printer::getInst()->printLog();
		if (model->m_firstReason != REASON_SUCCESS) { // failure occurred
			// Program failed- terminate for listed reason.
			Terminate::getInst()->exit(model->m_firstReason);
		}
	}
}
