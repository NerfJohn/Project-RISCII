/*
 * TargetUtil.h: Utilities specific to the target hardware.
 */

#ifndef UTIL_TARGETUTIL_H_
#define UTIL_TARGETUTIL_H_

#include <cstdint>
#include "Domain/RetErr_e.h"

// Definitions regarding target's memory capacity.
#define TARGET_ROM_MAX_BYTES (65536)
#define TARGET_RAM_MAX_BYTES (32768)
#define TARGET_BIN_MAX_BYTES (65536)

// Definitions regarding binary image headers/metadata.
#define TARGET_HDRS_SIZE      (4)      // bytes of image taken by headers
#define TARGET_SECT_MIN_BYTES (2)      // min size of text/data section each
#define TARGET_TEXT_HDR_BASE  (0x0000)
#define TARGET_DATA_HDR_BASE  (0x8000)

// TODO
void TargetUtil_addWord(uint16_t const word);

#endif /* UTIL_TARGETUTIL_H_ */
