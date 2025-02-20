/*
 * Lexer.cpp: Iteratively lexes pre-opened file.
 */

#include "Common/Device/File.h"

#include "Common/Device/Lexer.h"

using namespace std;

//==============================================================================

// Definitions for expected characters.
#define CHAR_NEWLINE (0x0A)

//==============================================================================
// Constructor- inits lexer for given input data.
Lexer::Lexer(LexInit_t const& initData) {
	// Save initializers.
	m_nextState = initData.m_nextState;
	m_isTkn     = initData.m_isTkn;
	m_lexStart  = initData.m_lexStart;
	m_lexError  = initData.m_lexError;
	
	// Preset internal/shareds.
	m_tknCode = m_lexStart;
	m_value   = "";
	m_line    = 1;              // assume fresh file
	m_curLine = 1;              // assume fresh file
}

//==============================================================================
// Lexes file for one token. Returns for token or lexing error.
void Lexer::lex(void) {
	// Reset shared data for new lex.
	m_tknCode = m_lexStart;
	m_value   = "";
	m_line    = m_curLine;
	
	// Prep call-specific metrics.
	bool isToken = false;
	
	// Scan until token (or error) found.
	while(isToken == false) {
		// Process next step.
		uint8_t peekByte  = File::inst().peek();
		        m_tknCode = m_nextState(m_tknCode, m_value, peekByte);
		        isToken   = m_isTkn(m_tknCode);
		
		// Handle buffer changes.
		if (isToken == false)        {m_value += File::inst().pop();}
		if (m_tknCode == m_lexStart) {m_value.clear();}
		
		// Handle line count changes.
		if ((peekByte == CHAR_NEWLINE) && (isToken == false)) {
			if (m_tknCode == m_lexStart) {m_line++;} // inc shared if finding...
			m_curLine++;                             // but always track value
		}
		
		// Exit early for lexing error.
		if (m_tknCode == m_lexError) {break;}
	}
}
