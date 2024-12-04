#include <gtest/gtest.h>
#include "Domain/RetCode_e.h"
#include "tests/Os/OsExit.h"
#include "tests/Os/OsStdout.h"

#include "Device/SymTable.h"

TEST(SymTable, addGetSymGood) {
	// Setup inputs.
	SymTable table;
	Symbol_t* symPtr = nullptr;
	Symbol_t sym;
	sym.m_name = "myName";
	
	// Call functions under test.
	RetErr_e addRet = table.addSym("myName", &sym);
	RetErr_e getRet = table.getSym("myName", symPtr);
	
	// Check results.
	EXPECT_EQ(addRet, RET_ERR_NONE);
	EXPECT_EQ(getRet, RET_ERR_NONE);
	EXPECT_EQ(symPtr, &sym);
	EXPECT_EQ(symPtr->m_name, "myName");
}

TEST(SymTable, addGetSymBadGet) {
	// Setup inputs.
	SymTable table;
	Symbol_t* symPtr = nullptr;
	Symbol_t sym;
	sym.m_name = "myName";
	
	// Call functions under test.
	RetErr_e addRet = table.addSym("myName", &sym);
	RetErr_e getRet = table.getSym("notMyName", symPtr);
	
	// Check results.
	EXPECT_EQ(addRet, RET_ERR_NONE);
	EXPECT_EQ(getRet, RET_ERR_ERROR);
	EXPECT_EQ(symPtr, nullptr);
}

TEST(SymTable, addSymTwice) {
	// Setup inputs.
	SymTable table;
	Symbol_t sym;
	sym.m_name = "myName";
	
	// Call functions under test.
	RetErr_e addRet1 = table.addSym("myName", &sym);
	RetErr_e addRet2 = table.addSym("myName", &sym);
	
	// Check results.
	EXPECT_EQ(addRet1, RET_ERR_NONE);
	EXPECT_EQ(addRet2, RET_ERR_ERROR);
}

TEST(SymTable, addSymNull) {
	// Setup inputs.
	SymTable table;
	
	// Call functions under test.
	table.addSym("myName", nullptr);
	
	// Check results.
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_ASSERT);
}

TEST(SymTable, asList) {
	// Setup inputs.
	SymTable table;
	Symbol_t sym;
	std::vector<Symbol_t*> list;
	sym.m_name = "myName";
	
	// Call function under test.
	table.asList(list);
	
	// Check results.
	EXPECT_EQ(list.size(), 0);
	
	// Call function again with new inputs.
	table.addSym("myName", &sym);
	table.asList(list);
	
	// Check results again.
	EXPECT_EQ(list.size(), 1);
	EXPECT_EQ(list[0], &sym);
}