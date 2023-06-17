/*
 * MemHandler.h
 */

#ifndef MEMHANDLER_H_
#define MEMHANDLER_H_

#include "AsmMaker.h"
#include "Symbol.h"

// TODO
#define REG_FILE_SIZE 8
#define NUM_LOAD_REGS 4

// TODO
enum KeyType_e {
	KEY_SYMBOL,
	KEY_LITERAL,
	KEY_INTERMEDIATE,
};

// TODO
typedef struct genValue {
	KeyType_e type;
	int data;
} genValue_t;

// TODO
class MemHandler {
public:
	// TODO
	static RegName_e load(AsmMaker* asmGen, genValue_t key);

	// TODO
	static void loadLitToReg(AsmMaker* asmGen, RegName_e reg, int lit);

	// TODO
	static void store(AsmMaker* asmGen, RegName_e reg, genValue_t key);

	// TODO
	static void updateReg(RegName_e reg, genValue_t key);
	static void clearLoads(void);

private:
	// Private constructor to prevent instantiation.
	MemHandler(void) {/* Empty ctor */}

	// TODO
	static genValue_t m_regFile[REG_FILE_SIZE];

	// TODO
	static int m_loadUses[NUM_LOAD_REGS];

	// TODO
	static void usedReg(RegName_e reg);

	// TODO
	static void loadFromOffset(AsmMaker* asmGen, RegName_e reg, int offset);
};

#endif /* MEMHANDLER_H_ */
