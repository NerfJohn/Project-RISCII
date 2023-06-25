/*
 * MemHandler.h
 */

#ifndef MEMHANDLER_H_
#define MEMHANDLER_H_

#include "AsmMaker.h"
#include "Symbol.h"

// Sizes defined by convention/hardware architecture.
#define REG_FILE_SIZE 8
#define NUM_LOAD_REGS 4

// Enum to specify type of item in the register file.
enum KeyType_e {
	KEY_SYMBOL,
	KEY_LITERAL,
	KEY_INTERMEDIATE,
};

// Struct to define value being loaded/stored in the register file registers.
typedef struct genValue {
	KeyType_e type;
	int data;
} genValue_t;

// Static class to simplify translation of loads/stores.
class MemHandler {
public:
	// Generates code to load desired value.
	static RegName_e load(AsmMaker* asmGen, genValue_t key);

	// Generates code to load a given literal to a given register.
	static void loadLitToReg(AsmMaker* asmGen, RegName_e reg, int lit);

	// Generates code to store a register's contents.
	static void store(AsmMaker* asmGen, RegName_e reg, genValue_t key);

	// Generates code to push an argument in a given register to the stack.
	static void pushArg(AsmMaker* asmGen, RegName_e reg, int offset);

	// Helper functions to manually control the register file records.
	static void updateReg(RegName_e reg, genValue_t key);
	static void clearLoads(void);
	static void clearAccum(void);

private:
	// Private constructor to prevent instantiation.
	MemHandler(void) {/* Empty ctor */}

	// Register file mock-up to determine correct instruction to generate.
	static genValue_t m_regFile[REG_FILE_SIZE];

	// Usage records of load registers to optimize their use.
	static int m_loadUses[NUM_LOAD_REGS];

	// Helper function to update usage records of load registers.
	static void usedReg(RegName_e reg);

	// Helper function to load a value in via an offset from the frame pointer.
	static void loadFromOffset(AsmMaker* asmGen, RegName_e reg, int offset);
};

#endif /* MEMHANDLER_H_ */
