/*
 * TargetUtil.h: Utilities specific to the target hardware.
 */

#ifndef UTIL_TARGETUTIL_H_
#define UTIL_TARGETUTIL_H_

#include <cstdint>
#include "Domain/RetErr_e.h"

// Definitions regarding target's memory capacity.
#define TARGET_RAM_MAX_BYTES (32768)
#define TARGET_BIN_MAX_BYTES (65536)

// Definitions regarding binary image headers/metadata.
#define TARGET_HDRS_SIZE      (4)      // bytes of image taken by headers
#define TARGET_SECT_MIN_BYTES (2)      // min size of text/data section each

/*
 * Appends a given word to (presumed open) image file.
 *
 * Appends word to image file using target specific endianess. Assumes image
 * file is already open for writing.
 *
 * @param word value to append to the image file
 */
void TargetUtil_addWord(uint16_t const word);

/*
 * Appends text section header to (presumed open) image file.
 *
 * Computes and adds text section header word to image file. Assumes image file
 * is not only already open for writing, but that the size of the text section
 * (ie the param) has been checked for correctness.
 *
 * @param numBytes number of bytes taken up by the text section
 */
void TargetUtil_addTextHeader(uint16_t const numBytes);

/*
 * Appends data section header to (presumed open) image file.
 *
 * Computes and adds data section header word to image file. Assumes image file
 * is not only already open for writing, but that the size of the data section
 * (ie the param) has been checked for correctness.
 *
 * @param numBytes number of bytes taken up by the data section
 */
void TargetUtil_addDataHeader(uint16_t const numBytes);

#endif /* UTIL_TARGETUTIL_H_ */
