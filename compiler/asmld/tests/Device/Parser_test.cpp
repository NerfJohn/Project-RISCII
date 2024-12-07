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

TEST(Parser, parseEmpty) {
	// Prep input.
	std::stack<ParseState_e> stack;
	
	// Attempt a bad parse.
	Parser_parse(stack, TOKEN_COMMENT);

	// Check operation resulted in assert.
	std::string output = "ASSERT! Parsed with empty stack\n";
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

TEST(Parser, eof) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(PARSE_FILE));
	
	// Attempt a bad parse.
	RetErr_e retErr = Parser_parse(stack, TOKEN_EOF);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.top(), TOKEN_EOF);
}

TEST(Parser, shrKeyword) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(PARSE_FILE));
	
	// Attempt a bad parse.
	RetErr_e retErr = Parser_parse(stack, TOKEN_KW_SHR);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.top(), TOKEN_KW_SHR);
	stack.pop();
	EXPECT_EQ(stack.top(), PARSE_OPT_FLAGS);
	stack.pop();
	EXPECT_EQ(stack.top(), TOKEN_REGISTER);
	stack.pop();
	EXPECT_EQ(stack.top(), TOKEN_REGISTER);
	stack.pop();
	EXPECT_EQ(stack.top(), PARSE_REG_IMM);
	stack.pop();
	EXPECT_EQ(stack.top(), ACTION_INSTR);
}

TEST(Parser, optFlag) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(PARSE_OPT_FLAGS));
	
	// Attempt a bad parse.
	RetErr_e retErr = Parser_parse(stack, TOKEN_REGISTER);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.size(), 0);
	
	// Prep input.
	stack.push((ParseState_e)(PARSE_OPT_FLAGS));
	
	// Attempt a bad parse.
	retErr = Parser_parse(stack, TOKEN_FLAGS);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.size(), 1);
	EXPECT_EQ(stack.top(), TOKEN_FLAGS);
}

TEST(Parser, regImm) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(PARSE_REG_IMM));
	
	// Attempt a bad parse.
	RetErr_e retErr = Parser_parse(stack, TOKEN_REGISTER);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.size(), 1);
	EXPECT_EQ(stack.top(), TOKEN_REGISTER);
	
	// Prep input.
	stack.pop();
	stack.push((ParseState_e)(PARSE_REG_IMM));
	
	// Attempt a bad parse.
	retErr = Parser_parse(stack, TOKEN_IMMEDIATE);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.size(), 1);
	EXPECT_EQ(stack.top(), TOKEN_IMMEDIATE);
}

TEST(Parser, dataKeyword) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(PARSE_FILE));
	
	// Attempt a bad parse.
	RetErr_e retErr = Parser_parse(stack, TOKEN_KW_DATA);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.top(), TOKEN_KW_DATA);
	stack.pop();
	EXPECT_EQ(stack.top(), PARSE_INIT_DATA);
	stack.pop();
	EXPECT_EQ(stack.top(), ACTION_DATA);
	stack.pop();
	EXPECT_EQ(stack.top(), PARSE_FILE);
}

TEST(Parser, initDataImm) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(PARSE_INIT_DATA));
	
	// Attempt a bad parse.
	RetErr_e retErr = Parser_parse(stack, TOKEN_IMMEDIATE);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.size(), 1);
	EXPECT_EQ(stack.top(), TOKEN_IMMEDIATE);
}

TEST(Parser, initDataLabel) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(PARSE_INIT_DATA));
	
	// Attempt a bad parse.
	RetErr_e retErr = Parser_parse(stack, TOKEN_LABEL);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.size(), 1);
	EXPECT_EQ(stack.top(), TOKEN_LABEL);
}

TEST(Parser, initDataStrLit) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(PARSE_INIT_DATA));
	
	// Attempt a bad parse.
	RetErr_e retErr = Parser_parse(stack, TOKEN_STRLIT);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.size(), 1);
	EXPECT_EQ(stack.top(), TOKEN_STRLIT);
}

TEST(Parser, initDataLcurly) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(PARSE_INIT_DATA));
	
	// Attempt a bad parse.
	RetErr_e retErr = Parser_parse(stack, TOKEN_LCURLY);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.top(), TOKEN_LCURLY);
	stack.pop();
	EXPECT_EQ(stack.top(), PARSE_INIT_LIST);
}

TEST(Parser, initListImm) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(PARSE_INIT_LIST));
	
	// Attempt a bad parse.
	RetErr_e retErr = Parser_parse(stack, TOKEN_IMMEDIATE);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.top(), TOKEN_IMMEDIATE);
	stack.pop();
	EXPECT_EQ(stack.top(), PARSE_INIT_LIST);
}

TEST(Parser, initListLabel) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(PARSE_INIT_LIST));
	
	// Attempt a bad parse.
	RetErr_e retErr = Parser_parse(stack, TOKEN_LABEL);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.top(), TOKEN_LABEL);
	stack.pop();
	EXPECT_EQ(stack.top(), PARSE_INIT_LIST);
}

TEST(Parser, initListStrLit) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(PARSE_INIT_LIST));
	
	// Attempt a bad parse.
	RetErr_e retErr = Parser_parse(stack, TOKEN_STRLIT);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.top(), TOKEN_STRLIT);
	stack.pop();
	EXPECT_EQ(stack.top(), PARSE_INIT_LIST);
}

TEST(Parser, initListRcurly) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(PARSE_INIT_LIST));
	
	// Attempt a bad parse.
	RetErr_e retErr = Parser_parse(stack, TOKEN_RCURLY);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.size(), 1);
	EXPECT_EQ(stack.top(), TOKEN_RCURLY);
}

TEST(Parser, labelDecl) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(PARSE_FILE));
	
	// Attempt a bad parse.
	RetErr_e retErr = Parser_parse(stack, TOKEN_LABEL);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.top(), TOKEN_LABEL);
	stack.pop();
	EXPECT_EQ(stack.top(), TOKEN_COLON);
	stack.pop();
	EXPECT_EQ(stack.top(), ACTION_DECL);
	stack.pop();
	EXPECT_EQ(stack.top(), PARSE_FILE);
}

TEST(Parser, modifierGlobal) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(PARSE_FILE));
	
	// Attempt a bad parse.
	RetErr_e retErr = Parser_parse(stack, TOKEN_KW_GLOBAL);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.top(), TOKEN_KW_GLOBAL);
	stack.pop();
	EXPECT_EQ(stack.top(), TOKEN_LABEL);
	stack.pop();
	EXPECT_EQ(stack.top(), ACTION_MOD);
	stack.pop();
	EXPECT_EQ(stack.top(), PARSE_FILE);
}

TEST(Parser, modifierWeak) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(PARSE_FILE));
	
	// Attempt a bad parse.
	RetErr_e retErr = Parser_parse(stack, TOKEN_KW_WEAK);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.top(), TOKEN_KW_WEAK);
	stack.pop();
	EXPECT_EQ(stack.top(), TOKEN_LABEL);
	stack.pop();
	EXPECT_EQ(stack.top(), ACTION_MOD);
	stack.pop();
	EXPECT_EQ(stack.top(), PARSE_FILE);
}

TEST(Parser, laKeyword) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(PARSE_FILE));
	
	// Attempt a bad parse.
	RetErr_e retErr = Parser_parse(stack, TOKEN_KW_LA);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.top(), TOKEN_KW_LA);
	stack.pop();
	EXPECT_EQ(stack.top(), TOKEN_REGISTER);
	stack.pop();
	EXPECT_EQ(stack.top(), PARSE_LBL_IMM);
	stack.pop();
	EXPECT_EQ(stack.top(), ACTION_FUNC);
	stack.pop();
	EXPECT_EQ(stack.top(), PARSE_FILE);
}

TEST(Parser, bssKeyword) {
	// Prep input.
	std::stack<ParseState_e> stack;
	stack.push((ParseState_e)(PARSE_FILE));
	
	// Attempt a bad parse.
	RetErr_e retErr = Parser_parse(stack, TOKEN_KW_BSS);

	// Check final parsing results.
	EXPECT_EQ(retErr, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Check stack.
	EXPECT_EQ(stack.top(), TOKEN_KW_BSS);
	stack.pop();
	EXPECT_EQ(stack.top(), TOKEN_IMMEDIATE);
	stack.pop();
	EXPECT_EQ(stack.top(), ACTION_DATA);
	stack.pop();
	EXPECT_EQ(stack.top(), PARSE_FILE);
}