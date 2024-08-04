/*
 * CheckProgramStep.cpp
 *
 * "Step of checking/analyzing parsed program as one unit"
 */

#include "DeviceLayer/LabelTable.h"
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

	// Check label table for error/warnings.
	model->m_labelTable.validateTable(*model);

	// Ensure at least one data word exists (requirement of binary/hardware).
	if (model->m_numDataBytes == 0) {
		// Add dummy word.
		model->m_dataSection.push_back(0);
		model->m_numDataBytes += TARGETUTILS_WORD_SIZE;

		// (Inform debugging users).
		string dbgStr = string("No data section- adding \"0x0000\" to meet ") +
				        "target's binary requirements";
		Printer::getInst()->log(LOG_DEBUG, dbgStr);
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

	// Check if projected data section will fit (in memory/RAM).
	uint32_t dataSize    = model->m_numDataBytes + model->m_numBssBytes;
	uint32_t maxMemSize = TARGETUTILS_MAX_MEM_SIZE;
	if (dataSize > maxMemSize) {
		// Report sizing of text section.
		string errStr = string("Data/Bss section(s) are too large- ")
				        + to_string(dataSize)
						+ " > "
						+ to_string(maxMemSize)
						+ " bytes";
		Printer::getInst()->log(LOG_ERR, errStr);
		ErrorUtils_includeReason(model, REASON_BIG_DATABSS);
	}

	// Check if projected binary image is within target's size.
	uint32_t binSize    = model->m_numTextBytes +
			              model->m_numDataBytes +
						  TARGETUTILS_METADATA_SIZE;
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

	// (Inform info users...)
	string infoStr1 = string("Image = ") +
			         to_string(binSize) +
					 "/" +
					 to_string(TARGETUTILS_MAX_BIN_SIZE) +
					 " bytes (Text = " +
					 to_string(model->m_numTextBytes) +
					 ", Data = " +
					 to_string(model->m_numDataBytes) +
					 ")";
	string infoStr2 = string("RAM = ") +
			          to_string(dataSize) +
					  "/" +
					  to_string(TARGETUTILS_MAX_MEM_SIZE) +
					  " bytes (Data = " +
					  to_string(model->m_numDataBytes) +
					  ", Bss = " +
					  to_string(model->m_numBssBytes) +
					  ")";
	Printer::getInst()->log(LOG_INFO, infoStr1);
	Printer::getInst()->log(LOG_INFO, infoStr2);

	// Handle "outputs" of step.
	Printer::getInst()->printLog();
	if (model->m_firstReason != REASON_SUCCESS) { // failure occurred
		// Program failed- terminate for listed reason.
		Printer::getInst()->printSummary(*model);
		Printer::getInst()->printLog();
		Terminate::getInst()->exit(model->m_firstReason);
	}
}
