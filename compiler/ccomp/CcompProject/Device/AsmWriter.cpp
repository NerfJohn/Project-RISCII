/*
 * AsmWriter.cpp: TODO
 */

#include "Common/Device/File.h"
#include "Common/Device/Print.h"
#include "Common/Util/Msg.h"

#include "Device/AsmWriter.h"

using namespace std;

//==============================================================================

// Definitions for range checking values.
#define IN_I6(x)      ((-32 <= (x)) && ((x) <= 31))
#define IN_I8(x)      ((-128 <= (x)) && ((x) <= 127))
#define IN_B16(x)     ((-32768 <= (x)) && ((x) <= 65535))
#define IN_SET(a,x,b) (((a) <= (x)) && ((x) <= (b)))

//==============================================================================
// Helper functions to validate [label] arguments.
bool AsmWriter::isLabel(std::string const& label) {
	// Result.
	bool res = true;

	// Check.
	for (size_t i = 0; i < label.size(); i++) {
		// Get.
		char chr = label[i];

		// Analyze.
		bool isFirst  = (0 == i);
		bool isUscore = ('_' == chr);
		bool isNumber = IN_SET('0',chr,'9');
		bool isLetter = IN_SET('a',chr,'z') || IN_SET('A',chr,'Z');

		// Check.
		if      (isFirst && !isUscore && !isLetter)   {res = false;}
		else if (!isUscore && !isLetter && !isNumber) {res = false;}
	}

	// Return.
	return res;
}

//==============================================================================
// TODO
AsmWriter::AsmWriter(std::string& fname) {
	// Prep file.
	m_isOpen = (RET_ERR_ERROR == File_open(fname, FILE_OP_WRITE));
}

//==============================================================================
// TODO
bool AsmWriter::fileOpened(void) {
	// Result.
	return m_isOpen;
}

//==============================================================================
// TODO
RetErr_e AsmWriter::labelLocal(std::string const& lbl) {
	// Result.
	RetErr_e res = RET_ERR_NONE;

	// Print.
	if(this->isLabel(lbl)) {
		File_write(LF); // formatter
		File_write(Msg() + lbl + ":" + LF);
	}
	else {res = RET_ERR_ERROR;}

	// Return.
	return res;
}

//==============================================================================
// TODO
RetErr_e AsmWriter::labelGlobal(std::string const& lbl) {
	// Result.
	RetErr_e res = RET_ERR_NONE;

	// Print.
	if(this->isLabel(lbl)) {
		File_write(LF); // formatter
		File_write(Msg() + ".global " + lbl + LF);
		File_write(Msg() + lbl + ":" + LF);
	}
	else {res = RET_ERR_ERROR;}

	// Return.
	return res;
}

//==============================================================================
// TODO
RetErr_e AsmWriter::dataBss(int32_t size) {
	// Result.
	RetErr_e res = RET_ERR_NONE;

	// Print.
	if(size >= 0) {
		File_write(Msg() + ".bss " + size + LF);
		File_write(LF); // formatter
	}
	else {res = RET_ERR_ERROR;}

	// Return.
	return res;
}

//==============================================================================
// TODO
RetErr_e AsmWriter::dataInit(std::deque<int32_t> const& list) {
	// Result.
	RetErr_e res = RET_ERR_NONE;

	// Check.
	bool areValid = true;
	for (int32_t lit : list) {if (false == IN_B16(lit)) {areValid = false;}}

	// Print.
	if (areValid) {
		File_write(".data { ");
		for (int32_t lit : list) {File_write(Msg() + lit + " ");}
		File_write(Msg() + "}" + LF);
		File_write(LF); // formatter
	}
	else {res = RET_ERR_ERROR;}

	// Return.
	return res;
}

//==============================================================================
// TODO
RetErr_e AsmWriter::loadLit(AsmWrReg_e wreg, int32_t lit) {
	// Result.
	RetErr_e res = RET_ERR_NONE;

	// Print.
	if      (IN_I8(lit))  {
		File_write(Msg() + "LBI       $" + wreg + "    " + lit + LF);
	}
	else if (IN_B16(lit)) {
		File_write(Msg() + "_la       $" + wreg + "    " + lit + LF);
	}
	else {res = RET_ERR_ERROR;}

	// Return.
	return res;
}

//==============================================================================
// TODO
RetErr_e AsmWriter::loadLit(AsmWrReg_e wreg, std::string const& lbl) {
	// Result.
	RetErr_e res = RET_ERR_NONE;

	// Print.
	if (this->isLabel(lbl)) {
		File_write(Msg() + "_la       $" + wreg + "    " + lbl + LF);
	}
	else {res = RET_ERR_ERROR;}

	// Return.
	return res;
}

//==============================================================================
// TODO
RetErr_e AsmWriter::loadVal(AsmWrReg_e wreg, AsmWrReg_e base, int32_t off) {
	// Result.
	RetErr_e res = RET_ERR_NONE;

	// Print.
	if (IN_I6(off)) {
		File_write(Msg() + "LDR       $" + wreg + " $" + base + " " + off + LF);
	}
	else {res = RET_ERR_ERROR;}

	// Return.
	return res;
}

//==============================================================================
// TODO
AsmWriter::~AsmWriter(void) {
	File_close();
}
