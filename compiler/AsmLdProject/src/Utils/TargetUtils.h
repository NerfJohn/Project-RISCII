/*
 * TargetUtils.h
 *
 * "Helper functions for tasks involving target specific information"
 */

#ifndef SRC_UTILS_TARGETUTILS_H_
#define SRC_UTILS_TARGETUTILS_H_

#include <string>
#include "DomainLayer/DataWordType_e.h"
#include "DomainLayer/InstrType_e.h"
#include "DomainLayer/LexToken_e.h"

// Public definitions of sizes (in bytes) related to target.
#define TARGETUTILS_INSTR_SIZE     (2)
#define TARGETUTILS_MAX_TEXT_SIZE  (65530)  // 64 KB - metadata - 1 data word
#define TARGETUTILS_MAX_MEM_SIZE   (32768)  // Data + Bss must fit in 32 KB RAM
#define TARGETUTILS_METADATA_SIZE  (4)      // instruction and data word
#define TARGETUTILS_MAX_BIN_SIZE   (65536)  // 64 KB image maximum
#define TARGETUTILS_WORD_SIZE      (2)      // Bytes per word

// Public definitions for key base numbers.
#define TARGETUTILS_TEXT_META_BASE (0x0000) // base of text section metadata
#define TARGETUTILS_DATA_META_BASE (0x8000) // base of data section metadata

/*
 * Converts token into instruction type. Returns INSTR_INVALID on failure.
 *
 * General translation function to turn a lex token into a more restricted
 * instruction type (based off of lex token keywords).
 *
 * @param token lex token to turn into an instruction type
 * @return instruction type if successful, INSTR_INVALID otherwise
 */
InstrType_e TargetUtils_asInstr(LexToken_e token);

/*
 * Validates a bit flag for a given instruction.
 *
 * @param instr type of instruction flag is being used for/on
 * @param flag symbol/byte being used to refer to a specific flag
 * @return true if flag can be used with the instruction, false otherwise
 */
bool TargetUtils_isValidFlag(InstrType_e instr, uint8_t flag);

/*
 * Gets string of valid flag options for an instruction.
 *
 * Returns possible flags for given instruction within a string. This function
 * is primarily meant for making printed messages more verbose.
 *
 * @param instr instruction the returned flags can be used for/on
 * @return string of char flags that go with the instruction
 */
std::string TargetUtils_getInstrFlags(InstrType_e instr);

/*
 * Validates if uint is within range of a register.
 *
 * Bound checks a uint (preferably from a register variable) for the target's
 * supported register file registers.
 *
 * @param reg uint "name" of the register
 * @return true if uint refers to a valid register, false otherwise
 */
bool TargetUtils_isValidReg(uint32_t reg);

/*
 * Gets type/range of register as string.
 *
 * Returns the type for registers- denoting their acceptable range. This
 * function is primarily meant for making printed messages more verbose.
 *
 * @return string of type/range acceptable for registers
 */
std::string TargetUtils_getRegType(void);

/*
 * Validates if int is within valid range for a given instruction.
 *
 * @param instr instruction containing the immediate
 * @param imm immediate value being evaluated
 * @return true if immediate can be used with instruction, false otherwise
 */
bool TargetUtils_isValidImm(InstrType_e instr, int32_t imm);

/*
 * Gets type/range of immediate for given instruction as string.
 *
 * Returns type/range of immediate supported by the instruction. This function
 * is primarily meant for making printed messages more verbose.
 *
 * @param instr instruction hosting immediate type
 * @return type/range of immediate supported by instruction
 */
std::string TargetUtils_getImmType(InstrType_e instr);

/*
 * Validates if int is within valid range for given data word.
 *
 * @param range specifies intended range of data word
 * @param word word value being evaluated
 * @return true if word is within range, false otherwise
 */
bool TargetUtils_isValidWord(DataWordType_e range, int32_t word);

/*
 * Gets type/range specified as a string.
 *
 * Returns type/range as a string. This function is primarily meant for making
 * printed messages more verbose.
 *
 * @param range range to string-ify
 * @return type/range of specified range
 */
std::string TargetUtils_getWordType(DataWordType_e range);

#endif /* SRC_UTILS_TARGETUTILS_H_ */
