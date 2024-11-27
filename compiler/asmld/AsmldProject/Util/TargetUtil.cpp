/*
 * TargetUtil.cpp: Utilities specific to the target hardware.
 */

#include "Device/File.h"
#include "Util/IsaUtil.h"

#include "Util/TargetUtil.h"

using namespace std;

//==============================================================================

// Definitions for computing section headers.
#define TARGET_TEXT_HDR_BASE  (0x0000)
#define TARGET_DATA_HDR_BASE  (0x8000)

//==============================================================================
// Appends a given word to (presumed open) image file.
void TargetUtil_addWord(uint16_t const word) {
	// Break word into two bytes for writing.
	uint8_t upperByte = (word >> 8) & 0xFF;
	uint8_t lowerByte =  word       & 0xFF;

	// Write bytes MSB (uP specified- hence the function).
	File::inst().write(upperByte);
	File::inst().write(lowerByte);
}

//==============================================================================
// Appends text section header to (presumed open) image file.
void TargetUtil_addTextHeader(uint16_t const numBytes) {
	// Compute text section header.
	uint16_t header = ((numBytes - ISA_WORD_BYTES) / 2) + TARGET_TEXT_HDR_BASE;

	// Add word to file.
	TargetUtil_addWord(header);
}

//==============================================================================
// Appends data section header to (presumed open) image file.
void TargetUtil_addDataHeader(uint16_t const numBytes) {
	// Compute text section header.
	uint16_t header = ((numBytes - ISA_WORD_BYTES) / 2) + TARGET_DATA_HDR_BASE;

	// Add word to file.
	TargetUtil_addWord(header);
}
