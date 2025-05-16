/*
 * AsmWriter.h: TODO
 */

#ifndef DEVICE_ASMWRITER_H_
#define DEVICE_ASMWRITER_H_

// TODO- move
typedef enum {
	// Core Values.
	ASMWRITER_OP_ADD
} AsmWrOp_e;

// TODO- move
typedef enum {
	// Core Values.
	ASMWRITER_REG_L0 = 0,
	ASMWRITER_REG_L1 = 1,
	ASMWRITER_REG_L2 = 2,
	ASMWRITER_REG_L3 = 3,
	ASMWRITER_REG_IV = 4,
	ASMWRITER_REG_AV = 5,
	ASMWRITER_REG_FP = 6,
	ASMWRITER_REG_SP = 7
} AsmWrReg_e;

#include <deque>
#include <string>
#include "Common/Domain/RetErr_e.h"

// TODO- writes asm to file, effectively a "toString;toFile" class
class AsmWriter {
public:
	// TODO
	AsmWriter(std::string& fname);

	// TODO
	bool fileOpened(void);

	// TODO
	RetErr_e labelLocal(std::string const& lbl);
	RetErr_e labelGlobal(std::string const& lbl);

	// TODO
	RetErr_e dataBss(int32_t size);
	RetErr_e dataInit(std::deque<int32_t> const& list);

	// TODO
	RetErr_e loadLit(AsmWrReg_e wreg, int32_t lit);
	RetErr_e loadLit(AsmWrReg_e wreg, std::string const& lbl);
	RetErr_e loadVal(AsmWrReg_e wreg, AsmWrReg_e base, int32_t off);

	// TODO
	RetErr_e operation(AsmWrReg_e dst, AsmWrReg_e src1, int32_t lit);
	RetErr_e operation(AsmWrReg_e dst, AsmWrReg_e src1, AsmWrReg_e src2);

	// TODO
	RetErr_e saveReg(AsmWrReg_e dreg, AsmWrReg_e sreg);
	RetErr_e saveVal(AsmWrReg_e sreg, AsmWrReg_e base, int32_t off);

	// TODO
	~AsmWriter(void);
private:
	// Vars regarding managed file.
	bool m_isOpen; // "open" result

	// Helper functions to validate arguments.
	bool isLabel(std::string const& label);
};

#endif /* DEVICE_ASMWRITER_H_ */
