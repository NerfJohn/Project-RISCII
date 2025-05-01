/*
 * Lexer.cpp: Iteratively lexes given file.
 */

#include "Common/Device/File.h"
#include "Common/Util/LexerDefs.h"

#include "Common/Device/Lexer.h"

using namespace std;

//==============================================================================

// Definitions for special/expected characters.
#define CHAR_NEWLINE (0x0A)

//==============================================================================
// Constructor- opens specified file for lexing.
Lexer::Lexer(std::string const& fname) {
	// Open file for reading.
	m_fileOpened = (File_open(fname, FILE_OP_READ) == RET_ERR_NONE);
	m_curLine    = 1; // naturally begin with first line

	// Haven't lexed yet- nothing to share.
	m_tknCode = LEXER_ERROR;
	m_value   = "";
	m_line    = 0;
}

//==============================================================================
// Getter for result of file read operation.
bool Lexer::fileOpened(void) {return m_fileOpened;}

//==============================================================================
// Lexes file for one token. Returns for token or lexing error.
void Lexer::lex(void) {
	// Reset shared data for new lex.
	m_tknCode = LEXER_START;
	m_value   = "";
	m_line    = m_curLine;
	
	// Prep call-specific metrics.
	bool isToken = false;
	
	// Scan until token (or error) found.
	while(isToken == false) {
		// Process next step.
		uint8_t peekByte  = File_peek();
		        m_tknCode = nextState(m_tknCode, m_value, peekByte);
		        isToken   = isTkn(m_tknCode);
		
		// Handle buffer changes.
		if (isToken == false)         {m_value += File_pop();}
		if (m_tknCode == LEXER_START) {m_value.clear();}
		
		// Handle line count changes.
		if ((peekByte == CHAR_NEWLINE) && (isToken == false)) {
			if (m_tknCode == LEXER_START) {m_line++;} // still finding token?
			m_curLine++;                              // regardless, track line
		}
		
		// Exit early for lexing error.
		if (m_tknCode == LEXER_ERROR) {break;}
	}
}

//==============================================================================
// Destructor- closes file prior to destruction.
Lexer::~Lexer(void) {File_close();}
