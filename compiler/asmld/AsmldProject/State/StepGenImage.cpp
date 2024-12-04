/*
 * StepGenImage.cpp: Step/process to generate binary image from data model.
 */

#include "Device/File.h"
#include "Device/Print.h"
#include "Device/Terminate.h"
#include "Ds/AAsmNode.h"
#include "Util/IsaUtil.h"
#include "Util/ModelUtil.h"
#include "Util/TargetUtil.h"

#include "State/StepGenImage.h"

using namespace std;

//==============================================================================

// Macros for easing reading of checks in step execution.
#define LOG_BIN_ERR_STR  (string("Image is too large- ")    + \
				          to_string(binSize)                + \
						  " > "                             + \
						  to_string(TARGET_BIN_MAX_BYTES))
#define LOG_RAM_ERR_STR  (string("Ram usage is too high- ") + \
				          to_string(ramSize)                + \
						  " > "                             + \
						  to_string(TARGET_RAM_MAX_BYTES))

// Macros for easing reading of summary in step execution.
#define LOG_NAME_STR     (string("\tName:  ") + model.m_outFile)
#define LOG_BIN_INFO_STR (string("\tImage: ")               + \
                          to_string(binSize)                + \
                          "/"                               + \
						  to_string(TARGET_BIN_MAX_BYTES)   + \
						  " bytes ("                        + \
						  to_string(model.m_textSize)       + \
						  " text, "                         + \
						  to_string(model.m_dataSize)       + \
						  " data)")
#define LOG_RAM_INFO_STR (string("\tRam:   ")               + \
                          to_string(ramSize)                + \
                          "/"                               + \
						  to_string(TARGET_RAM_MAX_BYTES)   + \
						  " bytes ("                        + \
						  to_string(model.m_dataSize)       + \
						  " data, "                         + \
						  to_string(model.m_bssSize)        + \
						  " bss)")

//==============================================================================
// Helper function to open a file for writing (with info/error handling).
static RetErr_e StepGenImage_openWriteFile(DataModel_t& model) {
	// Result of process (optimistic).
	RetErr_e retErr = RET_ERR_NONE;

	// Get name of write file.
	string fname = model.m_outFile;

	// (Stay informative.)
	Print::inst().log(LOG_INFO, string("Opening '") + fname + "'...");

	// Actually open file.
	retErr = File::inst().open(fname, FILE_OP_WRITE);

	// Record error on occurrence.
	if (retErr) {
		Print::inst().log(LOG_ERROR, string("Can't write '") + fname + "'");
		ModelUtil_recordError(model, RET_NO_WRIT);
	}

	// Return result of process.
	return retErr;
}

//==============================================================================
// Executes process to generate binary image from model.
void StepGenImage_execute(DataModel_t& model) {
	// (Inform User.)
	Print::inst().log(LOG_INFO, "=Gen Image=");

	// TODO- "rotate" nodes so first node is start of the program.

	// Generate the binary sections (FIRST the labels, THEN the values).
	for (AAsmNode* node : model.m_nodes) {
		IF_NULL(node, "Addressed null node");
		node->imageAddress(model);
	}
	// TODO- update assembler defined labels
	for (AAsmNode* node : model.m_nodes) {
		IF_NULL(node, "Assembled null node");
		node->imageAssemble(model);
	}

	// Target requires at least one word in each section.
	if (model.m_textSize < TARGET_SECT_MIN_BYTES) {
		// Generating with no "start" instruction? compiler bug.
		Terminate_assert("Generated 0 instruction image");
	}
	if (model.m_dataSize < TARGET_SECT_MIN_BYTES) {
		Print::inst().log(LOG_INFO, "Data section required- adding 0x0000");
		model.m_dataVals.push_back(0x0000);
		model.m_dataSize += ISA_WORD_BYTES;
	}

	// (Sanity check "word precision" of ISA).
	if (model.m_textSize % ISA_WORD_BYTES) {Terminate_assert("Odd text bytes");}
	if (model.m_dataSize % ISA_WORD_BYTES) {Terminate_assert("Odd data bytes");}
	if (model.m_bssSize % ISA_WORD_BYTES)  {Terminate_assert("Odd bss bytes");}

	// Perform final checks prior to image creation.
	uint32_t binSize = model.m_textSize + model.m_dataSize + TARGET_HDRS_SIZE;
	uint32_t ramSize = model.m_dataSize + model.m_bssSize;
	if (binSize > TARGET_BIN_MAX_BYTES) {
		Print::inst().log(LOG_ERROR, LOG_BIN_ERR_STR);
		ModelUtil_recordError(model, RET_BIG_BIN);
	}
	if (ramSize > TARGET_RAM_MAX_BYTES) {;
		Print::inst().log(LOG_ERROR, LOG_RAM_ERR_STR);
		ModelUtil_recordError(model, RET_BIG_RAM);
	}

	// If no complaints- create the binary image.
	if (model.m_numErrs == 0) {
		// Open/create image file.
		RetErr_e retErr = StepGenImage_openWriteFile(model);

		// Write file (as able).
		if (retErr == RET_ERR_NONE) {
			// TODO- add debug symbols if asked.

			// Write text section.
			TargetUtil_addTextHeader(model.m_textSize);
			for (uint16_t word : model.m_textVals) {TargetUtil_addWord(word);}

			// Write data section.
			TargetUtil_addDataHeader(model.m_dataSize);
			for (uint16_t word : model.m_dataVals) {TargetUtil_addWord(word);}

			// Log summary of created file.
			Print::inst().log(LOG_INFO, "=Image Summary=");
			Print::inst().log(LOG_INFO, LOG_NAME_STR);
			Print::inst().log(LOG_INFO, LOG_BIN_INFO_STR);
			Print::inst().log(LOG_INFO, LOG_RAM_INFO_STR);
		}

		// binary written- close file.
		File::inst().close();
	}

	// Terminate program (with summary) if errors were found.
	if (model.m_numErrs > 0) {Terminate_summary(model);}
}
