#include <gtest/gtest.h>

#include "Util/IsaUtil.h"

TEST(IsaUtil, asInstrGood) {
	// Call utility under test.
	InstrType_e res = IsaUtil_asInstr(TOKEN_KW_SHR);
	
	// Check results.
	EXPECT_EQ(res, INSTR_SHR);
}

TEST(IsaUtil, asInstrBad) {
	// Call utility under test.
	InstrType_e res = IsaUtil_asInstr(TOKEN_COMMENT);
	
	// Check results.
	EXPECT_EQ(res, INSTR_INVALID);
}

TEST(IsaUtil, isValidFlagBad) {
	// Call utility under test.
	bool res = IsaUtil_isValidFlag(INSTR_SHR, (uint8_t)('q'));
	
	// Check results.
	EXPECT_EQ(res, false);
}

TEST(IsaUtil, isValidFlagException) {
	// Call utility under test.
	bool res = IsaUtil_isValidFlag(INSTR_SHR, (uint8_t)('%'));
	
	// Check results.
	EXPECT_EQ(res, true);
}

TEST(IsaUtil, isValidFlagSHRa) {
	// Call utility under test.
	bool res = IsaUtil_isValidFlag(INSTR_SHR, (uint8_t)('a'));
	
	// Check results.
	EXPECT_EQ(res, true);
}

TEST(IsaUtil, toRegBadRange) {
	// Call utility under test.
	uint8_t regInt = 0;
	RetErr_e res = IsaUtil_toReg("$8", regInt);
	
	// Check results.
	EXPECT_EQ(res, RET_ERR_ERROR);
}

TEST(IsaUtil, toRegGood) {
	// Call utility under test.
	uint8_t regInt = 0;
	RetErr_e res = IsaUtil_toReg("$4", regInt);
	
	// Check results.
	EXPECT_EQ(res, RET_ERR_NONE);
	EXPECT_EQ(regInt, 4);
}

TEST(IsaUtil, toImmBadStr) {
	// Call utility under test.
	Imm_t immInt;
	RetErr_e res = IsaUtil_toImm("$4", immInt);
	
	// Check results.
	EXPECT_EQ(res, RET_ERR_ERROR);
}

TEST(IsaUtil, toImmGoodDecimal) {
	// Call utility under test.
	Imm_t immInt;
	RetErr_e res = IsaUtil_toImm("-5", immInt);
	
	// Check results.
	EXPECT_EQ(res, RET_ERR_NONE);
	EXPECT_EQ(immInt.m_val, -5);
	EXPECT_EQ(immInt.m_isBin, false);
}

TEST(IsaUtil, toImmGoodHex) {
	// Call utility under test.
	Imm_t immInt;
	RetErr_e res = IsaUtil_toImm("0xA", immInt);
	
	// Check results.
	EXPECT_EQ(res, RET_ERR_NONE);
	EXPECT_EQ(immInt.m_val, 10);
	EXPECT_EQ(immInt.m_isBin, true);
}

TEST(IsaUtil, isValidImmBad) {
	// Prep input.
	Imm_t immInt;
	immInt.m_val = -5;
	immInt.m_isBin = false;
	
	// Call utility.
	bool res = IsaUtil_isValidImm(INSTR_SHR, immInt);
	
	// Check results.
	EXPECT_EQ(res, false);
}

TEST(IsaUtil, isValidImmShr) {
	// Prep input.
	Imm_t immInt;
	immInt.m_val = 15;
	immInt.m_isBin = false;
	
	// Call utility.
	bool res = IsaUtil_isValidImm(INSTR_SHR, immInt);
	
	// Check results.
	EXPECT_EQ(res, true);
	
	// re-do with "hex".
	immInt.m_isBin = true;
	res = IsaUtil_isValidImm(INSTR_SHR, immInt);

	// Check results.
	EXPECT_EQ(res, true);
}