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

TEST(SymTable, labelSTART) {
	// Setup inputs.
	SymTable table;
	Symbol_t* sym = new Symbol_t();
	sym->m_name     = SYM_START_NAME;
	sym->m_isGlobal = false;
	sym->m_numRefs  = 0;
	
	// Check start symbol isn't defined.
	RetErr_e ret = table.getSym(SYM_START_NAME, sym);
	EXPECT_EQ(ret, RET_ERR_ERROR);
	
	// Check defining it automatically makes it global + referenced.
	ret = table.addSym(SYM_START_NAME, sym);
	EXPECT_EQ(ret, RET_ERR_NONE);
	EXPECT_EQ(sym->m_isGlobal, true);
	EXPECT_EQ(sym->m_numRefs, 1);
}

TEST(SymTable, labelSIZE) {
	// Setup inputs.
	SymTable table;
	Symbol_t* sym = nullptr;
	
	// Check symbol is already present in (new) table.
	RetErr_e ret = table.getSym("__SIZE", sym);
	EXPECT_EQ(ret, RET_ERR_NONE);
	EXPECT_NE(sym, nullptr);
	
	// Check symbol defaults.
	EXPECT_EQ(sym->m_name, "__SIZE");
	EXPECT_EQ(sym->m_file, "asmld_predef");
	EXPECT_EQ(sym->m_line, 0);
	EXPECT_EQ(sym->m_isGlobal, true);
	EXPECT_EQ(sym->m_numRefs, 1);
	
	// Check all tables share the same instance.
	SymTable table2;
	Symbol_t* sym2 = nullptr;
	table2.getSym("__SIZE", sym2);
	EXPECT_EQ(sym, sym2);

	// Check it cannot be re-added.
	ret = table.addSym("__SIZE", sym);
	EXPECT_EQ(ret, RET_ERR_ERROR);	
}

TEST(SymTable, labelBSS) {
	// Setup inputs.
	SymTable table;
	Symbol_t* sym = nullptr;
	
	// Check symbol is already present in (new) table.
	RetErr_e ret = table.getSym("__BSS", sym);
	EXPECT_EQ(ret, RET_ERR_NONE);
	EXPECT_NE(sym, nullptr);
	
	// Check symbol defaults.
	EXPECT_EQ(sym->m_name, "__BSS");
	EXPECT_EQ(sym->m_file, "asmld_predef");
	EXPECT_EQ(sym->m_line, 0);
	EXPECT_EQ(sym->m_isGlobal, true);
	EXPECT_EQ(sym->m_numRefs, 1);
	
	// Check all tables share the same instance.
	SymTable table2;
	Symbol_t* sym2 = nullptr;
	table2.getSym("__BSS", sym2);
	EXPECT_EQ(sym, sym2);

	// Check it cannot be re-added.
	ret = table.addSym("__BSS", sym);
	EXPECT_EQ(ret, RET_ERR_ERROR);	
}

TEST(SymTable, labelFREE) {
	// Setup inputs.
	SymTable table;
	Symbol_t* sym = nullptr;
	
	// Check symbol is already present in (new) table.
	RetErr_e ret = table.getSym("__FREE", sym);
	EXPECT_EQ(ret, RET_ERR_NONE);
	EXPECT_NE(sym, nullptr);
	
	// Check symbol defaults.
	EXPECT_EQ(sym->m_name, "__FREE");
	EXPECT_EQ(sym->m_file, "asmld_predef");
	EXPECT_EQ(sym->m_line, 0);
	EXPECT_EQ(sym->m_isGlobal, true);
	EXPECT_EQ(sym->m_numRefs, 1);
	
	// Check all tables share the same instance.
	SymTable table2;
	Symbol_t* sym2 = nullptr;
	table2.getSym("__FREE", sym2);
	EXPECT_EQ(sym, sym2);

	// Check it cannot be re-added.
	ret = table.addSym("__FREE", sym);
	EXPECT_EQ(ret, RET_ERR_ERROR);	
}
