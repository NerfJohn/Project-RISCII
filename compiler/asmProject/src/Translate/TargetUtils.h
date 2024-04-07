/*
 * TargetUtils.h
 *
 *  Created on: Apr 6, 2024
 *      Author: John
 */

#ifndef SRC_TRANSLATE_TARGETUTILS_H_
#define SRC_TRANSLATE_TARGETUTILS_H_

#include <stdint.h>
#include <string>
#include <vector>
#include "DataItem.h"
#include "InstrItem.h"
#include "../Parsing/SyntaxToken_e.h"

// TODO- Memory access precision. Useful for data organization.
#define TARGET_ACCESS_SIZE 2

// TODO- max program sizes for target hardware.
#define TARGET_ROM_MAX (0x10000 - 6) // 3 words- .text size, .data size, checksum
#define TARGET_RAM_MAX (0x8000)

// TODO- Sizes value to fir within one word (which happens to be one memory access).
int TargetUtils_sizeToWord(int value);

// TODO- Sizes value for given instruction (ie to instruction's immediate requirements).
int TargetUtils_sizeToInstr(int value, SyntaxToken_e instr);

// TODO- confirms/denies if flag can be used with given instruction.
bool TargetUtils_instrUsesFlag(SyntaxToken_e instr,char flag);

// TODO- Converts flags into bit-mask intended for ORing with translated instruction.
int TargetUtils_flagsToField(SyntaxToken_e instr, std::string flagStr);

// TODO- Converts instruction into 16-bit binary instruction.
uint16_t TargetUtils_toBinInstr(InstrItem* instrItem);

#endif /* SRC_TRANSLATE_TARGETUTILS_H_ */
