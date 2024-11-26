/*
 * IsaUtil.h: Utilities derived from the implemented ISA.
 */

#ifndef UTIL_ISAUTIL_H_
#define UTIL_ISAUTIL_H_

#include <string>
#include "Domain/Imm_t.h"
#include "Domain/Instr_t.h"
#include "Domain/InstrType_e.h"
#include "Domain/LexToken_e.h"
#include "Domain/RetErr_e.h"

// Definitions for ISA defined sizes.
#define ISA_WORD_BYTES (2)

// TODO
RetErr_e IsaUtil_genInstr(uint16_t& instr, Instr_t const& fields);

/*
 * Converts lex token into specific instruction opcode.
 *
 * Attempts to turn lex token enum into instruction opcode enum. This not only
 * confirms if a lex token is an opcode, but effectively gets the opcode's true
 * value (ie enum value is explicitly set). INSTR_INVALID used if not an opcode.
 *
 * @param tkn lex token to convert into an instruction opcode
 * @return    instruction opcode on success, INSTR_INVALID otherwise
 */
InstrType_e IsaUtil_asInstr(LexToken_e const tkn);

/*
 * Confirms if flag is valid for specified instruction.
 *
 * Implements logic tying specific flags to specific opcodes. For convenience,
 * the character '%' always returns TRUE (ie intended to be used directly on a
 * flags var- %[a-z]+ -eases caller's string manipulation).
 *
 * @param instr instruction opcode flag is paired with
 * @param flag  flag being checked for validity w/ respect to opcode
 * @return      TRUE if flag is valid (or char is '%'), FALSE otherwise
 */
bool IsaUtil_isValidFlag(InstrType_e const instr, uint8_t const flag);

/*
 * Converts register token string into an integer.
 *
 * Implements logic to extract the integer value of a register. For convenience,
 * function handles the register prefix (ie works around '$' register prefix)
 * and factors for valid register range in return value (ie if register exists).
 *
 * @param regStr token string representing a register
 * @param regInt integer to populate with the register's integer value
 * @return       RET_ERR_NONE is successful, RET_ERR_ERROR otherwise
 */
RetErr_e IsaUtil_toReg(std::string const& regStr, uint8_t& regInt);

/*
 * Converts immediate token string into an Imm_t type.
 *
 * Extracts value from string, populating the given Imm_t instance with the
 * value. Also specifies if the string implied an integer value vs a binary
 * value (ie implied sign vs no direct sign- implied in Imm_t).
 *
 * @param immStr token string representing an immediate
 * @param immint Imm_t type to populate with the immediate's value/properties
 * @return       RET_ERR_NONE is successful, RET_ERR_ERROR
 */
RetErr_e IsaUtil_toImm(std::string const& immStr, Imm_t& immInt);

/*
 * Confirms if immediate is within valid range for specified instruction.
 *
 * Implements logic tying specific immediate sizes to specific opcodes. Function
 * factors for both the immediate's raw value and it's direct/indirect sign.
 *
 * @param instr  instruction opcode immediate is paired with
 * @param immInt immediate being checked for validity w/ respect to the opcode
 * @return       TRUE if flag is valid, FALSE otherwise
 */
bool IsaUtil_isValidImm(InstrType_e const instr, Imm_t const& immInt);

#endif /* UTIL_ISAUTIL_H_ */
