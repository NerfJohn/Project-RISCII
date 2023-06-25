/*
 * MemHandler.cpp
 */

#include "MemHandler.h"

#include "MsgLog.h"

using namespace std;

// Internally defined constants for ease of processing.
constexpr KeyType_e EMPTY_REG 		= (KeyType_e)(-1);
constexpr KeyType_e RESERVED_REG 	= (KeyType_e)(-2);

// Bounds of load register usage values- LRU scheme.
constexpr int REG_RECENTLY_USED = 3;
constexpr int REG_TO_EVICT = 0;

// Register file mock-up to determine correct instruction to generate.
genValue_t MemHandler::m_regFile[REG_FILE_SIZE] = {
	{.type=EMPTY_REG}, {.type=EMPTY_REG},
	{.type=EMPTY_REG}, {.type=EMPTY_REG},
	{.type=RESERVED_REG}, {.type=RESERVED_REG},
	{.type=EMPTY_REG}, {.type=RESERVED_REG}
};

// Usage records of load registers to optimize their use.
int MemHandler::m_loadUses[NUM_LOAD_REGS] = {
		REG_TO_EVICT, REG_TO_EVICT,
		REG_TO_EVICT, REG_TO_EVICT,
};

// Helper function to update usage records of load registers.
void MemHandler::usedReg(RegName_e reg) {
	// Don't consider if not a load register.
	if (reg > REG_3) {return;}

	// Otherwise, demote all above used register (to make room at top).
	int oldVal = m_loadUses[reg];
	for (int i = 0; i < NUM_LOAD_REGS; i++) {
		if (m_loadUses[i] > oldVal) {m_loadUses[i]--;}
	}

	// Promote used value to fill hole at top.
	m_loadUses[reg] = REG_RECENTLY_USED;
}

// Helper function to load a value in via an offset from the frame pointer.
void MemHandler::loadFromOffset(AsmMaker* asmGen, RegName_e reg, int offset) {
	// Either directly load it (if small enough) or use spare register.
	if ((-32 <= offset) && (offset <= 30)) {
		// Directly load.
		asmGen->genInstr({.type=INSTR_LDR,.r1=(RegName_e)reg,
			.r2=REG_FP,.imm=offset});
	}
	else {
		// Otherwise, use indirect pointer to access.
		MemHandler::loadLitToReg(asmGen, REG_RA, offset);
		asmGen->genInstr({.type=INSTR_ADD,.r1=REG_RA,
			.r2=REG_RA,.r3=REG_FP});
		asmGen->genInstr({.type=INSTR_LDR,.r1=(RegName_e)reg,
			.r2=REG_RA,.imm=0});
	}
}

// Generates code to load desired value.
RegName_e MemHandler::load(AsmMaker* asmGen, genValue_t key) {
	// Check if already in register file.
	for (int i = 0; i < REG_FILE_SIZE; i++) {
		genValue_t reg = m_regFile[i];
		if ((reg.type == key.type) && (reg.data == key.data)) {
			// Value found- track register's use and report finding.
			RegName_e retReg = (RegName_e)(i);
			MemHandler::usedReg(retReg);
			return (RegName_e)(retReg);
		}
	}

	// Will have to load- determine which load register to use.
	int loadReg = REG_0;
	for (loadReg = 0; loadReg < NUM_LOAD_REGS; loadReg++) {
		if (m_loadUses[loadReg] == REG_TO_EVICT) {break;}
	}

	// Process load based on type of key being loaded.
	if (key.type == KEY_LITERAL) {
		/* Data represents literal value to load */

		// Simple- load the literal.
		MemHandler::loadLitToReg(asmGen, (RegName_e)loadReg, key.data);
	}
	else if (key.type == KEY_INTERMEDIATE) {
		// Data irrelevant- intermediates are popped onto stack.
		string cmt = "Pop intermediate from stack";
		asmGen->genInstr({.type=INSTR_LDR, .r1=(RegName_e)loadReg,
			.r2=REG_SP, .imm=0});
		asmGen->genInstr({.type=INSTR_ADD,.r1=REG_SP,
			.r2=REG_SP,.imm=2,.cmt=cmt});
	}
	else {
		/* Data represents Symbol ptr */

		// Get valid pointer to provided Symbol.
		Symbol* sym = (Symbol*)(key.data);

		// Determine process based on symbol's details.
		if (sym->m_isGlobal) {
			// Memory value is physical address.
			MemHandler::loadLitToReg(asmGen, REG_RA, sym->m_memValue);
			asmGen->genInstr({.type=INSTR_LDR,.r1=(RegName_e)loadReg,
						.r2=REG_RA,.imm=0});
		}
		else if ((-128 <= sym->m_constVal) && (sym->m_constVal <= 127)) {
			// Symbol has easy constant value.
			MemHandler::loadLitToReg(asmGen,
					(RegName_e)loadReg, sym->m_constVal);

			MsgLog::logINFO("OPT- replacing load of \"" + sym->m_id +
					"\" with " + to_string(sym->m_constVal));
		}
		else {
			// Memory value is offset from frame pointer.
			MemHandler::loadFromOffset(asmGen,
					(RegName_e)loadReg, sym->m_memValue);
		}
	}

	// Key should be loaded now- update maps for tracking.
	MemHandler::usedReg((RegName_e)loadReg);
	m_regFile[loadReg] = key;

	// And finally- return the loaded register.
	return (RegName_e)loadReg;
}

// Generates code to load a given literal to a given register.
void MemHandler::loadLitToReg(AsmMaker* asmGen, RegName_e reg, int lit) {
	// Generate set instructions based on size of number needed.
	string cmt = to_string(lit) + " -> r" + to_string(reg);
	if (lit == (int8_t)(lit)) {
		// One instruction needed.
		asmGen->genInstr({.type=INSTR_LBI,.r1=reg,.imm=lit,.cmt=cmt});
	}
	else {
		// Two instructions needed.
		int msb = (0xff00 & lit) >> 8;
		int lsb = 0xff & lit;
		asmGen->genInstr({.type=INSTR_LBI,.r1=reg,.imm=msb,.cmt=cmt});
		asmGen->genInstr({.type=INSTR_LBI,.flg=FLAG_SHIFT,.r1=reg,.imm=lsb});
	}
}

// Generates code to store a register's contents.
void MemHandler::store(AsmMaker* asmGen, RegName_e reg, genValue_t key) {
	// Process store depending on type of value (note: literals aren't stored).
	if (key.type == KEY_INTERMEDIATE) {
		// Data irrelevant- intermediates are pushed to stack.
		string cmt = "Push intermediate to stack";
		asmGen->genInstr({.type=INSTR_ADD,.r1=REG_SP,
			.r2=REG_SP,.imm=-2,.cmt=cmt});
		asmGen->genInstr({.type=INSTR_STR, .r1=(RegName_e)reg,
			.r2=REG_SP, .imm=0});
	}
	else if (key.type == KEY_SYMBOL) {
		/* Data represents symbol ptr */

		// Get valid pointer to provided Symbol.
		Symbol* sym = (Symbol*)(key.data);

		// Determine process based on symbol's details.
		if (sym->m_isGlobal) {
			// Memory value is physical address.
			MemHandler::loadLitToReg(asmGen, REG_RA, sym->m_memValue);
			asmGen->genInstr({.type=INSTR_STR,.r1=(RegName_e)reg,
						.r2=REG_RA,.imm=0});
		}
		else {
			// Memory value is offset from frame pointer.
			int offset = sym->m_memValue;
			if ((-32 <= offset) && (offset <= 30)) {
				// Directly store.
				asmGen->genInstr({.type=INSTR_STR,.r1=(RegName_e)reg,
					.r2=REG_FP,.imm=offset});
			}
			else {
				// Otherwise, use indirect pointer to access.
				MemHandler::loadLitToReg(asmGen, REG_RA, offset);
				asmGen->genInstr({.type=INSTR_ADD,.r1=REG_RA,
					.r2=REG_RA,.r3=REG_FP});
				asmGen->genInstr({.type=INSTR_STR,.r1=(RegName_e)reg,
					.r2=REG_RA,.imm=0});
			}
		}
	}
}

// Generates code to push an argument in a given register to the stack.
void MemHandler::pushArg(AsmMaker* asmGen, RegName_e reg, int offset) {
	// Memory value is offset from frame pointer.
	if ((-32 <= offset) && (offset <= 30)) {
		// Directly store.
		asmGen->genInstr({.type=INSTR_STR,.r1=(RegName_e)reg,
			.r2=REG_SP,.imm=offset});
	}
	else {
		// Otherwise, use indirect pointer to access.
		MemHandler::loadLitToReg(asmGen, REG_RA, offset);
		asmGen->genInstr({.type=INSTR_ADD,.r1=REG_RA,
			.r2=REG_RA,.r3=REG_SP});
		asmGen->genInstr({.type=INSTR_STR,.r1=(RegName_e)reg,
			.r2=REG_RA,.imm=0});
	}
}

// Helper function to manually update a register's contents.
void MemHandler::updateReg(RegName_e reg, genValue_t key) {
	// First- invalid any stale references (namely stale symbol values).
	if (key.type == KEY_SYMBOL) {
		for (int i = 0; i < REG_FILE_SIZE; i++) {
			genValue_t oldReg = m_regFile[i];
			if ((oldReg.type == key.type) && (oldReg.data == key.data)) {
				// Value found- mark register as empty.
				oldReg.type = EMPTY_REG;
				m_regFile[i] = oldReg;
			}
		}
	}

	// Overwrite register with given contents.
	m_regFile[reg] = key;
}

// Helper function to manually clear all load register records.
void MemHandler::clearLoads(void) {
	// Reset load register values.
	genValue_t emptyReg = {.type=EMPTY_REG};
	m_regFile[0] = emptyReg;
	m_regFile[1] = emptyReg;
	m_regFile[2] = emptyReg;
	m_regFile[3] = emptyReg;

}

// Helper function to manually clear the accumulator register's record.
void MemHandler::clearAccum(void) {
	// Reset accumulator.
	genValue_t emptyReg = {.type=EMPTY_REG};
	m_regFile[REG_AC] = emptyReg;
}
