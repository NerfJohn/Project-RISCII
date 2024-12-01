#include <gtest/gtest.h>
#include "Domain/RetCode_e.h"
#include "tests/Os/OsExit.h"
#include "tests/Os/OsStdout.h"

#include "Device/SymTable.h"

TEST(SymTable, defineAssert) {
	// Setup inputs.
	ItemToken tkn;
	tkn.m_lexTkn = TOKEN_COMMENT; // ie NOT a label
	tkn.m_rawStr = "name";
	tkn.m_file   = "foobar";
	tkn.m_line   = 4;
	SymTable table;
	
	// Call function under test.
	Symbol_t* sym;
	RetErr_e ret = table.define(tkn, sym);
	
	// Check results.
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_ASSERT);
}

TEST(SymTable, defineNew) {
	// Setup inputs.
	ItemToken tkn;
	tkn.m_lexTkn = TOKEN_LABEL;
	tkn.m_rawStr = "name";
	tkn.m_file   = "foobar";
	tkn.m_line   = 4;
	SymTable table;
	
	// Call function under test.
	Symbol_t* sym;
	RetErr_e ret = table.define(tkn, sym);
	
	// Check results.
	EXPECT_NE(sym, nullptr);
	EXPECT_EQ(ret, RET_ERR_NONE);
	EXPECT_EQ(sym->m_file, tkn.m_file);
	EXPECT_EQ(sym->m_line, tkn.m_line);
}

TEST(SymTable, defineTwice) {
	// Setup inputs.
	ItemToken tkn;
	tkn.m_lexTkn = TOKEN_LABEL;
	tkn.m_rawStr = "name";
	tkn.m_file   = "foobar";
	tkn.m_line   = 4;
	SymTable table;
	
	// Call function under test.
	Symbol_t* sym;
	table.define(tkn, sym);
	tkn.m_line = 10;
	RetErr_e ret = table.define(tkn, sym);
	
	// Check results.
	EXPECT_NE(sym, nullptr);
	EXPECT_EQ(ret, RET_ERR_ERROR);
	EXPECT_EQ(sym->m_file, tkn.m_file);
	EXPECT_EQ(sym->m_line, 4);
}

TEST(SymTable, addSymGood) {
	// Setup inputs.
	SymTable table;
	Symbol_t sym;
	sym.m_name = "foo";
	
	// Call function under test.
	RetErr_e ret = table.addSym("foo", &sym);
	
	// Check results.
	EXPECT_EQ(ret, RET_ERR_NONE);
}

TEST(SymTable, addSymBad) {
	// Setup inputs.
	SymTable table;
	Symbol_t sym;
	sym.m_name = "foo";
	
	// Call function under test.
	table.addSym("foo", &sym);
	RetErr_e ret = table.addSym("foo", &sym);
	
	// Check results.
	EXPECT_EQ(ret, RET_ERR_ERROR);
}

TEST(SymTable, getSymGood) {
	// Setup inputs.
	SymTable table;
	ItemToken tkn;
	Symbol_t* sym;
	tkn.m_lexTkn = TOKEN_LABEL;
	tkn.m_rawStr = "name";
	tkn.m_file   = "foobar";
	tkn.m_line   = 4;
	table.define(tkn, sym);
	
	// Ensure NOT global yet.
	EXPECT_EQ(sym->m_isGlobal, false);
	
	// Call function under test.
	RetErr_e ret = table.getSym(tkn.m_rawStr, sym);
	
	// Check results.
	EXPECT_NE(sym, nullptr);
	EXPECT_EQ(ret, RET_ERR_NONE);
}

TEST(SymTable, getSymBad) {
	// Setup inputs.
	SymTable table;
	ItemToken tkn;
	Symbol_t* sym;
	tkn.m_lexTkn = TOKEN_LABEL;
	tkn.m_rawStr = "name";
	tkn.m_file   = "foobar";
	tkn.m_line   = 4;
	table.define(tkn, sym);
	
	// Call function under test.
	RetErr_e ret = table.getSym("NOT HERE", sym);
	
	// Check results.
	EXPECT_NE(sym, nullptr);
	EXPECT_EQ(ret, RET_ERR_ERROR);
	EXPECT_EQ(OsExit_hasRet(), false);
}