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
	// First translate all items into binary sections.
	for (IBuildItem* item : model.m_items) {
		item->generateBinaryValue(&model);
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
	}

	// Handle additional "outputs" of the step.
	delete writeFile;
	Printer::getInst()->printLog();
	if (model.m_firstReason != REASON_SUCCESS) { // failure occurred
		// Program failed- terminate for listed reason.
		Terminate::getInst()->exit(model.m_firstReason);
	}
}
