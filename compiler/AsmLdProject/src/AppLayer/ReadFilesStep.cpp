/*
 * ReadFilesStep.cpp
 *
 * "Step of assembly process focused on reading each input file"
 */

#include "DeviceLayer/FileReader.h"
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
		FileReader* readFile = new FileReader(inFile);
		if (!readFile->isReady()) {
			// Log failure to open the file.
			string errStr = string("Failure to open file \"") + inFile + "\"";
			Printer::getInst()->log(LOG_ERR, errStr);
			ErrorUtils_includeReason(model, REASON_NO_FILE);
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
