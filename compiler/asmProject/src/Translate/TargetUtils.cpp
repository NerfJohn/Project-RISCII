/*
 * TargetUtils.cpp
 *
 *  Created on: Apr 6, 2024
 *      Author: John
 */

#include <stdint.h>

#include "TargetUtils.h"

using namespace std;

// TODO- Masks for zeroing non-target bits (or, inverted, forcing them high).
#define MASK_3_BIT 0x07
#define MASK_4_BIT 0x0f
#define MASK_5_BIT 0x1f
#define MASK_6_BIT 0x3f
#define MASK_8_BIT 0xff

// TODO- Quick operation to make 1 bit mask.
#define BIT(x) (1 << x)

// TODO- Quick operations for building instructions.
#define SHL(x, y) (x << y)

// TODO- Helper function- quick translation of opcode enum to opcode bits.
int TargetUtils_opcodeAsBits (SyntaxToken_e opcode) {
	// Return the correct opcode bit number.
	switch (opcode) {
	 case TOKEN_AND: return 0xF; break;
	 case TOKEN_ORR: return 0xE; break;
	 case TOKEN_XOR: return 0xB; break;
	 case TOKEN_SHL: return 0xC; break;
	 case TOKEN_SHR: return 0xD; break;
	 case TOKEN_ADD: return 0x8; break;
	 case TOKEN_SUB: return 0x9; break;
	 case TOKEN_LBI: return 0xA; break;
	 case TOKEN_LDR: return 0x5; break;
	 case TOKEN_STR: return 0x4; break;
	 case TOKEN_BRC: return 0x2; break;
	 case TOKEN_JLR: return 0x6; break;
	 case TOKEN_JPR: return 0x7; break;
	 case TOKEN_SWP: return 0x1; break;
	 case TOKEN_NOP: return 0x3; break;
	 case TOKEN_HLT: return 0x0; break;
	 default: /* bad apple */ return -1; break;
	}
}

// TODO- Sizes value to fir within one word (which happens to be one memory access).
int TargetUtils_sizeToWord(int value) {
	// Attempt to preserve sign/unsignedness- preserve int vs 16-bit comparison.
	int retVal = (value < 0) ? (int16_t)(value) : (uint16_t)(value);
	return retVal;
}

// TODO- Sizes value for given instruction (ie to instruction's immediate requirements).
int TargetUtils_sizeToInstr(int value, SyntaxToken_e instr) {
	// Returned int.
	int retInt = 0;

	// TODO- can't recognize hex-given values as either signed or unsigned

	// Size immediate based on instruction.
	switch (instr) {
		case TOKEN_SHL:
		case TOKEN_SHR:
			// unsigned 4-bit.
			retInt = value & MASK_4_BIT;
			break;
		case TOKEN_AND:
		case TOKEN_ORR:
		case TOKEN_XOR:
		case TOKEN_ADD:
		case TOKEN_SUB:
		case TOKEN_JPR:
		case TOKEN_JLR:
			// signed 5-bit.
			retInt = ((value < 0) || (value & BIT(4))) ? value | ~MASK_5_BIT : value & MASK_5_BIT;
			break;
		case TOKEN_LDR:
		case TOKEN_STR:
		case TOKEN_SWP:
			// signed 6-bit.
			retInt = ((value < 0) || (value & BIT(5))) ? value | ~MASK_6_BIT : value & MASK_6_BIT;
			break;
		case TOKEN_LBI:
		case TOKEN_BRC:
			// signed 8-bit.
			retInt = ((value < 0) || (value & BIT(7))) ? value | ~MASK_8_BIT : value & MASK_8_BIT;
			break;
		case TOKEN_NOP:
		case TOKEN_HLT:
		default:
			// No immediate.
			retInt = 0;
			break;
	}

	// Return converted value.
	return retInt;
}

// TODO- confirms/denies if flag can be used with given instruction.
bool TargetUtils_instrUsesFlag(SyntaxToken_e instr, char flag) {
	// Match valid chars based on instruction.
	switch (instr) {
		case TOKEN_SHR:
			return (flag == 'a');
			break;
		case TOKEN_LBI:
			return (flag == 's');
			break;
		case TOKEN_BRC:
			return ((flag == 'n') |
					(flag == 'z') |
					(flag == 'p') |
					(flag == 'c'));
			break;
		default:
			// Not a flag-able instruction.
			return false;
			break;
	}
}

// TODO- Converts flags into bit-mask intended for ORing with translated instruction.
int TargetUtils_flagsToField(SyntaxToken_e instr, std::string flagStr) {
	// Return bitmask.
	int retMask = 0;

	// Apply flags (as valid for the instruction).
	for (int i = 1; i < (int)(flagStr.size()); i++) { // i = 1, skip '%'
		// Get flag.
		char flag = flagStr[i];

		// Skip if invalid flag.
		if (!TargetUtils_instrUsesFlag(instr, flag)) {continue;}

		// Apply flag.
		switch (flag) {
			case 'a': retMask |= 0x0010; break;
			case 's': retMask |= 0x0100; break;
			case 'n': retMask |= 0x0800; break;
			case 'z': retMask |= 0x0400; break;
			case 'p': retMask |= 0x0200; break;
			case 'c': retMask |= 0x0100; break;
			default: break;
		}
	}

	// Return finished bitmask.
	return retMask;
}

// TODO- Converts instruction into 16-bit binary instruction.
uint16_t TargetUtils_toBinInstr(InstrItem* instrItem) {
	// Returned instruction.
	uint16_t retInstr = 0;

	// Break-out fields for ease of building any instruction.
	int op = TargetUtils_opcodeAsBits(instrItem->getOpcode());
	int fl = instrItem->getFlagMask();
	int r1 = instrItem->getRegIdx(0);
	int r2 = instrItem->getRegIdx(1);
	int r3 = instrItem->getRegIdx(2);
	int im = instrItem->getImmediate();

	// (Determine implicit flag/3rd operands for data operations.)
	int d3 = (r3 < 0) ? 0x20 | (im & MASK_5_BIT) : r3;
	int s3 = (r3 < 0) ? 0x20 | (im & MASK_4_BIT) : r3;

	// Build the instruction.
	retInstr = SHL(op,12); // Every instruction uses an opcode
	switch (instrItem->getOpcode()) {
		case TOKEN_AND:
		case TOKEN_ORR:
		case TOKEN_XOR:
		case TOKEN_ADD:
		case TOKEN_SUB:
			retInstr |= SHL(r1,9) | SHL(r2,6) | d3; break;
		case TOKEN_SHL:
		case TOKEN_SHR:
			retInstr |= SHL(r1,9) | s3; break;
		case TOKEN_LBI:
			retInstr |= SHL(r1,9) | (im & MASK_8_BIT); break;
		case TOKEN_LDR:
		case TOKEN_STR:
		case TOKEN_SWP:
			retInstr |= SHL(r1,9) | SHL(r2,6) | (im & MASK_6_BIT); break;
		case TOKEN_BRC:
			retInstr |= (im & MASK_8_BIT); break;
		case TOKEN_JLR:
			retInstr |= SHL(r1,9) | SHL(r2,6) | (im & MASK_5_BIT); break;
		case TOKEN_JPR:
			retInstr |= SHL(r1,6) | (im & MASK_5_BIT); break;
		case TOKEN_NOP:
		case TOKEN_HLT:
		default:
			retInstr |= /* just the opcode */ 0x0; break;
	}

	// Apply explicit flags.
	retInstr |= fl;

	// Return converted instruction.
	return retInstr;
}
