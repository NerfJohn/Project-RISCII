/*
 * CheckProgramStep.cpp
 *
 * "Step of checking/analyzing parsed program as one unit"
 */

#include "DeviceLayer/Printer.h"
#include "DeviceLayer/Terminate.h"
#include "Items/IBuildItem.h"
#include "Utils/ErrorUtils.h"
#include "Utils/TargetUtils.h"

#include "AppLayer/CheckProgramStep.h"

using namespace std;

//==============================================================================
// Checks parsed build items for program-level feasibility.
void CheckProgramStep_checkProgram(DataModel_t* model) {
	// First check/analyze all items together.
	for (IBuildItem* item : model->m_items) {
		item->doGlobalAnalysis(model);
	}

	// Check if projected text section will fit.
	uint32_t textSize    = model->m_numTextBytes;
	uint32_t maxTextSize = TARGETUTILS_MAX_TEXT_SIZE;
	if (textSize > maxTextSize) {
		// Report sizing of text section.
		string errStr = string("Text section is too large- ")
				        + to_string(textSize)
						+ " > "
						+ to_string(maxTextSize)
						+ " bytes";
		Printer::getInst()->log(LOG_ERR, errStr);
		ErrorUtils_includeReason(model, REASON_BIG_TEXT);
	}

	// Check if projected binary image is within target's size.
	uint32_t binSize    = model->m_numTextBytes + TARGETUTILS_METADATA_SIZE;
	uint32_t maxBinSize = TARGETUTILS_MAX_BIN_SIZE;
	if (binSize > maxBinSize) {
		// Report sizing of text section.
		string errStr = string("Binary image is too large- ")
				        + to_string(binSize)
						+ " > "
						+ to_string(maxBinSize)
						+ " bytes";
		Printer::getInst()->log(LOG_ERR, errStr);
		ErrorUtils_includeReason(model, REASON_BIG_BIN);
	}

	// Handle "outputs" of step.
	Printer::getInst()->printLog();
	if (model->m_firstReason != REASON_SUCCESS) { // failure occurred
		// Program failed- terminate for listed reason.
		Terminate::getInst()->exit(model->m_firstReason);
	}
}
