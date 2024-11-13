#include <gtest/gtest.h>
#include "Domain/RetCode_e.h"
#include "tests/Os/OsExit.h"
#include "tests/Os/OsStdout.h"

#include "Device/Parser.h"

TEST(Parser, asTkn) {
	// Attempt conversions.
	LexToken_e tkn1 = Parser_asLexToken((ParseState_e)(TOKEN_LEX_MIN - 1));
	LexToken_e tkn2 = Parser_asLexToken((ParseState_e)(TOKEN_LEX_MIN));
	LexToken_e tkn3 = Parser_asLexToken((ParseState_e)(TOKEN_LEX_MAX));
	LexToken_e tkn4 = Parser_asLexToken((ParseState_e)(TOKEN_LEX_MAX + 1));
	
	// Bounds check conversion to token.
	EXPECT_EQ(tkn1, TOKEN_INVALID);
	EXPECT_EQ(tkn2, TOKEN_LEX_MIN);
	EXPECT_EQ(tkn3, TOKEN_LEX_MAX);
	EXPECT_EQ(tkn4, TOKEN_INVALID);
}

TEST(Parser, asAction) {
	// Attempt conversions.
	ParseAction_e tkn1;
	ParseAction_e tkn2;
	ParseAction_e tkn3;
	ParseAction_e tkn4;
	tkn1 = Parser_asParseAction((ParseState_e)(ACTION_PARSE_MIN - 1));
	tkn2 = Parser_asParseAction((ParseState_e)(ACTION_PARSE_MIN));
	tkn3 = Parser_asParseAction((ParseState_e)(ACTION_PARSE_MAX));
	tkn4 = Parser_asParseAction((ParseState_e)(ACTION_PARSE_MAX + 1));
	
	// Bounds check conversion to token.
	EXPECT_EQ(tkn1, ACTION_INVALID);
	EXPECT_EQ(tkn2, ACTION_PARSE_MIN);
	EXPECT_EQ(tkn3, ACTION_PARSE_MAX);
	EXPECT_EQ(tkn4, ACTION_INVALID);
}

TEST(Parser, parseAssert) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(TOKEN_COMMENT));
	
	// Attempt a bad parse.
	Parser_parse(stack, TOKEN_COMMENT);

	// Check operation resulted in assert.
	std::string output = "ASSERT! Parsed with invalid top state\n";
	EXPECT_EQ(OsStdout_getOutput(), output);
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_ASSERT);
}

TEST(Parser, badParse) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(PARSE_FILE));
	
	// Attempt a bad parse.
	RetErr_e retErr = Parser_parse(stack, TOKEN_COMMENT);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_ERROR);
	EXPECT_EQ(OsExit_hasRet(), false);
}
