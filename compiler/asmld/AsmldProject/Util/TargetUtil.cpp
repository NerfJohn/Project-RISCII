/*
 * TargetUtil.cpp: Utilities specific to the target hardware.
 */

#include "Device/File.h"

#include "Util/TargetUtil.h"

using namespace std;

//==============================================================================
// TODO
void TargetUtil_addWord(uint16_t const word) {
	// Break word into two bytes for writing.
	uint8_t upperByte = (word >> 8) & 0xFF;
	uint8_t lowerByte =  word       & 0xFF;

	// Write bytes MSB (uP specified- hence the function).
	File::inst().write(upperByte);
	File::inst().write(lowerByte);
}
