/*
 * LexScanner.cpp
 *
 * "Device to conduct scanning/lexing of given read file"
 */

#include "DeviceLayer/Printer.h"
#include "Utils/ErrorUtils.h"
#include "Utils/LexUtils.h"

#include "DeviceLayer/LexScanner.h"

#include <iostream>

using namespace std;

//==============================================================================

// Definitions for expected characters.
#define CHAR_NEWLINE (0x0A)

//==============================================================================
// Getter for singleton instance. Instantiates on first call.
LexScanner* LexScanner::getInst(void) {
	// Static instance- implicitly creates one (and only one) instance.
	static LexScanner instance;

	// Return pointer to instance.
	return &instance;
}

//==============================================================================
// Scans read file into list of tokens. Returns nullptr on failure.
std::queue<ScanToken_t*>* LexScanner::scanFile(DataModel_t* model,
		                                      FileReader* file
											 ) {
	// Queue of tokens to return.
	queue<ScanToken_t*>* retTkns = new queue<ScanToken_t*>();

	// Track line numbers- good for warnings/errors.
	uint32_t lineNum = 1; // files start at line 1

	// Keep scanning until the end of the file.
	do {
		// "State" variables per token scanned.
		LexState_e lexState = LEX_START;
		string lexBuffer = "";
		LexToken_e lexToken = TOKEN_INVALID;

		// Lex bytes until a token shows up.
		do {
			// Check what the next state would be with peeked char.
			uint8_t peekByte = file->peekByte();
			lexState = LexUtils_nextState(lexState, peekByte);
			lexToken = LexUtils_asLexToken(lexState);

			// Take the appropriate path/procedure.
			if (lexState == LEX_START) {
				// Byte is not part of a token- skip without saving.
				file->popByte();
			}
			else if (lexState == LEX_ERROR) {
				// Buffer + peeked byte has no resolution- fatal error.
				string errStr = string("Unrecognized sequence \"")
						        + lexBuffer
								+ (char)(peekByte)
								+ "\"";
				Printer::getInst()->log(LOG_ERR,
						                file->getFilename(),
										lineNum,
										errStr
									   );
				ErrorUtils_includeReason(model, REASON_BAD_SCAN);

				// Fatal error- signal with nulled list.
				delete retTkns;
				return nullptr;
			}
			else if (lexToken == TOKEN_INVALID) {
				// Still parsing token- pop byte and append to save buffer.
				lexBuffer += file->popByte();
			}
			/* else- buffer represents a token; inner loop will break */

			// Adjust line number count as needed.
			if (peekByte == CHAR_NEWLINE) {lineNum++;}

		} while (lexToken == TOKEN_INVALID);

		// Skip tokenizing/save if a comment- not needed anymore.
		if (lexToken == TOKEN_COMMENT) {continue;}

		/* - Line reached- token is worth saving for parsing/analysis */

		// Create scan token from lexed data.
		ScanToken_t* newTkn  = new ScanToken_t();
		newTkn->m_lexTkn     = lexToken;
		newTkn->m_rawStr     = lexBuffer;
		newTkn->m_orignFile  = file->getFilename();
		newTkn->m_originLine = lineNum;

		// Append to maintain ordering.
		retTkns->push(newTkn);

	} while((retTkns->size() == 0) || (retTkns->back()->m_lexTkn != TOKEN_EOF));

	// File lexed successfully- return queue pointer (and thus ownership).
	return retTkns;
}
