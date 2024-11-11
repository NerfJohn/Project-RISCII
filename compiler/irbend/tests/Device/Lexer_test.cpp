#include <gtest/gtest.h>
#include "Domain/RetCode_e.h"
#include "tests/Os/OsExit.h"
#include "tests/Os/OsStdout.h"

#include "Device/Lexer.h"

TEST(Lexer, lexAssert) {
	// Attempt a bad lex.
	Lexer_nextState(LEX_ERROR, 0x00);

	// Check operation resulted in assert.
	std::string output = "ASSERT! Lexed from invalid starting state\n";
	EXPECT_EQ(OsStdout_getOutput(), output);
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_ASSERT);
}

TEST(Lexer, badLex) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "de ";     // no corresponding keyword
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, LEX_ERROR);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, whitespace) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = " \t\r\n";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, LEX_START);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, comment) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "// A comment for i8 i16 %d5\n";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_COMMENT);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, eof) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "\xff";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_EOF);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, decKeyword) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "dec ";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_KW_DEC);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, defKeyword) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "def\xff";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_KW_DEF);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, globKeyword) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "glob+";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_KW_GLOB);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, symbol) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "%0\n";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_SYMBOL);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Sample inputs.
	state = LEX_START;
	input = "%_foobar\r";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_SYMBOL);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Sample inputs.
	state = LEX_START;
	input = "%b33f_and_p0tat0es\xff";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_SYMBOL);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, intLiteral) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "13 ";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_INT_LIT);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Sample inputs.
	state = LEX_START;
	input = "-05q";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_INT_LIT);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, equalsOp) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "=a";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_EQUALS);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, lCurly) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "{i";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_L_CURLY);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, rCurly) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "}\xff";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_R_CURLY);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, commas) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = ",%";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_COMMA);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, i8Type) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "i8-";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_I8);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, i16Type) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "i16@";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_I16);
	EXPECT_EQ(OsExit_hasRet(), false);
}
