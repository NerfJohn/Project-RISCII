/*
 * GenerateImageStep.cpp
 *
 * "Translates program into binary image file"
 */

#include "DeviceLayer/FileWriter.h"
#include "DeviceLayer/Printer.h"
#include "DeviceLayer/Terminate.h"
#include "Items/IBuildItem.h"
#include "Utils/TargetUtils.h"

#include "AppLayer/GenerateImageStep.h"

using namespace std;

//==============================================================================

// Definitions to extract bytes from words.
#define UPPER_BYTE(x) (((x) >> 8) & 0xFF)
#define LOWER_BYTE(x) ((x) & 0xFF)

//==============================================================================
// Translates model's items into binary image- creating the file in the process.
void GenerateImageStep_generateImage(DataModel_t& model) {
	// Find "start" of program.
	size_t startIdx = 0;
	for (size_t i = 0; i < model.m_items.size(); i++) {
		if (model.m_items[i] == model.m_prgmStart) {startIdx = i;}
	}

	// Do 1st pass translation (in program order to generate addresses).
	for (size_t i = startIdx; i < model.m_items.size(); i++) {
		model.m_items[i]->generateBinaryValue(&model);
	}
	for (size_t i = 0; i < startIdx; i++) {
		model.m_items[i]->generateBinaryValue(&model);
	}

	// Do 2nd pass translation (to resolve labels- thus generating all values).
	for (IBuildItem* item : model.m_items) {
		item->resolveBinaryLabels(model);
	}

	// Open the file to become the binary image.
	FileWriter* writeFile = new FileWriter(model, model.m_outFile);

	// Populate the binary image (as applicable).
	if (writeFile->isOpen()) {
		// Text section meta data (last RAM address for text).
		uint16_t textMeta = TARGETUTILS_TEXT_META_BASE
				            + model.m_numTextBytes
							- TARGETUTILS_WORD_SIZE;
		writeFile->writeByte((uint8_t)(UPPER_BYTE(textMeta)));
		writeFile->writeByte((uint8_t)(LOWER_BYTE(textMeta)));

		// (Inform debugging users).
		string dbgStr = string("Written text metadata: ") + to_string(textMeta);
		Printer::getInst()->log(LOG_DEBUG, dbgStr);

		// Text section.
		for (uint16_t instr : model.m_textSection) {
			writeFile->writeByte((uint8_t)(UPPER_BYTE(instr)));
			writeFile->writeByte((uint8_t)(LOWER_BYTE(instr)));
		}

		// Data section + meta data // TODO- implement
		uint16_t dataMeta = TARGETUTILS_DATA_META_BASE;
		writeFile->writeByte((uint8_t)(UPPER_BYTE(dataMeta)));
		writeFile->writeByte((uint8_t)(LOWER_BYTE(dataMeta)));
		writeFile->writeByte(0x00);
		writeFile->writeByte(0x00);

		// (Inform debugging users).
		dbgStr = string("Written data metadata: ") + to_string(dataMeta);
		Printer::getInst()->log(LOG_DEBUG, dbgStr);

		// (Keep user informed).
		string infoStr = "Generated \"" + model.m_outFile + "\"";
		Printer::getInst()->log(LOG_INFO, infoStr);
	}

	// Handle additional "outputs" of the step.
	delete writeFile;
	Printer::getInst()->printLog();
	if (model.m_firstReason != REASON_SUCCESS) { // failure occurred
		// Program failed- terminate for listed reason.
		Printer::getInst()->printSummary(model);
		Printer::getInst()->printLog();
		Terminate::getInst()->exit(model.m_firstReason);
	}
}
