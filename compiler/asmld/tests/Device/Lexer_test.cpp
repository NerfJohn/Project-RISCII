#include <gtest/gtest.h>
#include "Domain/RetCode_e.h"
#include "tests/Os/OsExit.h"
#include "tests/Os/OsStdout.h"

#include "Device/Lexer.h"

TEST(Lexer, asTkn) {
	// Attempt conversions.
	LexToken_e tkn1 = Lexer_asToken((LexState_e)(TOKEN_LEX_MIN - 1));
	LexToken_e tkn2 = Lexer_asToken((LexState_e)(TOKEN_LEX_MIN));
	LexToken_e tkn3 = Lexer_asToken((LexState_e)(TOKEN_LEX_MAX));
	LexToken_e tkn4 = Lexer_asToken((LexState_e)(TOKEN_LEX_MAX + 1));
	
	// Bounds check conversion to token.
	EXPECT_EQ(tkn1, TOKEN_INVALID);
	EXPECT_EQ(tkn2, TOKEN_LEX_MIN);
	EXPECT_EQ(tkn3, TOKEN_LEX_MAX);
	EXPECT_EQ(tkn4, TOKEN_INVALID);
}

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
	// Sample inputs (ie no corresponding keyword).
	LexState_e  state = LEX_START;
	std::string input = "$q";
	
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
	std::string input = "; A comment for SHR %a $0 $1 3\n";
	
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

TEST(Lexer, flagVar) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "%a+";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_FLAGS);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, regVar) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "$3a";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_REGISTER);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, immVar) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "5q";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_IMMEDIATE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Sample inputs.
	state = LEX_START;
	input = "-00 ";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_IMMEDIATE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Sample inputs.
	state = LEX_START;
	input = "0xaB21}";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_IMMEDIATE);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, shrKeyword) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "SHR ";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, LEX_NAME);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, dataKeyword) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = ".data-";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, LEX_DIRECTIVE);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, label) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "_myLabel12=";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, LEX_NAME);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, colon) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "::";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, TOKEN_COLON);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, globalKeyword) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = ".global ";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, LEX_DIRECTIVE);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Lexer, laKeyword) {
	// Sample inputs.
	LexState_e  state = LEX_START;
	std::string input = "_la$";
	
	// Lex through entire string.
	while(input.size()) {
		state = Lexer_nextState(state, (uint8_t)(input[0]));
		input.erase(input.begin());
	}
	
	// Check final lexing results.
	EXPECT_EQ(state, LEX_NAME);
	EXPECT_EQ(OsExit_hasRet(), false);
}