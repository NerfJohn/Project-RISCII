#include <gtest/gtest.h>

#include "Device/GetOpt.h"

TEST(GetOpt, Init) {
	// Dummy inputs.
	int argc = 3;
	char* argv[] = {"asmld", "-h", "bar"};
	
	// Run constructor.
	GetOpt dut(argc, argv);
	
	// Check initial values.
	EXPECT_EQ(dut.m_asFlag, CLI_FLAG_INVALID);
	EXPECT_EQ(dut.m_type, CLI_TYPE_INVALID);
	EXPECT_EQ(dut.m_value, "");
	EXPECT_EQ(dut.m_arg, "");
}

TEST(GetOpt, FindFile) {
	// Input starts with "file".
	int argc = 3;
	char* argv[] = {"asmld", "-h", "bar"};
	
	// Run constructor.
	GetOpt dut(argc, argv);
	
	// Parse 1 token- should find something.
	EXPECT_EQ(dut.getOpt(), true);
	
	// Check updated values.
	EXPECT_EQ(dut.m_asFlag, CLI_FLAG_INVALID);
	EXPECT_EQ(dut.m_type, CLI_TYPE_FILE);
	EXPECT_EQ(dut.m_value, "asmld");
	EXPECT_EQ(dut.m_arg, "");
}

TEST(GetOpt, FindLoneFlag) {
	// Input starts with "long flag".
	int argc = 3;
	char* argv[] = {"-h", "loner", "bar"};
	
	// Run constructor.
	GetOpt dut(argc, argv);
	
	// Parse 1 token- should find something.
	EXPECT_EQ(dut.getOpt(), true);
	
	// Check updated values.
	EXPECT_EQ(dut.m_type, CLI_TYPE_LONE);
	EXPECT_EQ(dut.m_value, "-h");
	EXPECT_EQ(dut.m_arg, "");
}

TEST(GetOpt, FindArgFlag) {
	// Input starts with "arg flag".
	int argc = 3;
	char* argv[] = {"-o", "myArg", "bar"};
	
	// Run constructor.
	GetOpt dut(argc, argv);
	
	// Parse 1 token- should find something.
	EXPECT_EQ(dut.getOpt(), true);
	
	// Check updated values.
	EXPECT_EQ(dut.m_type, CLI_TYPE_ARG);
	EXPECT_EQ(dut.m_value, "-o");
	EXPECT_EQ(dut.m_arg, "myArg");
}

TEST(GetOpt, FindArgNone) {
	// Input starts with "arg" without argument.
	int argc = 1;
	char* argv[] = {"-o"};
	
	// Run constructor.
	GetOpt dut(argc, argv);
	
	// Parse 1 token- should find something.
	EXPECT_EQ(dut.getOpt(), true);
	
	// Check updated values.
	EXPECT_EQ(dut.m_type, CLI_TYPE_ARG);
	EXPECT_EQ(dut.m_value, "-o");
	EXPECT_EQ(dut.m_arg, "");
}

TEST(GetOpt, FindArgToFlag) {
	// Input starts with "arg" followed by flag.
	int argc = 2;
	char* argv[] = {"-o", "-h"};
	
	// Run constructor.
	GetOpt dut(argc, argv);
	
	// Parse 1 token- should find something.
	EXPECT_EQ(dut.getOpt(), true);
	
	// Check updated values.
	EXPECT_EQ(dut.m_type, CLI_TYPE_ARG);
	EXPECT_EQ(dut.m_value, "-o");
	EXPECT_EQ(dut.m_arg, "");
}

TEST(GetOpt, EndFile) {
	// Input ending with file.
	int argc = 1;
	char* argv[] = {"asmld"};
	
	// Run constructor/parse file.
	GetOpt dut(argc, argv);
	dut.getOpt();
	
	// Parse- should find nothing.
	EXPECT_EQ(dut.getOpt(), false);
	
	// Check updated values.
	EXPECT_EQ(dut.m_asFlag, CLI_FLAG_INVALID);
	EXPECT_EQ(dut.m_type, CLI_TYPE_INVALID);
	EXPECT_EQ(dut.m_value, "");
	EXPECT_EQ(dut.m_arg, "");
}

TEST(GetOpt, EndArg) {
	// Input ending with arg + argument.
	int argc = 2;
	char* argv[] = {"-o", "foo"};
	
	// Run constructor/parse file.
	GetOpt dut(argc, argv);
	dut.getOpt();
	
	// Parse- should find nothing.
	EXPECT_EQ(dut.getOpt(), false);
	
	// Check updated values.
	EXPECT_EQ(dut.m_asFlag, CLI_FLAG_INVALID);
	EXPECT_EQ(dut.m_type, CLI_TYPE_INVALID);
	EXPECT_EQ(dut.m_value, "");
	EXPECT_EQ(dut.m_arg, "");
}

TEST(GetOpt, NullStr) {
	// Input with "null" string.
	int argc = 1;
	char* argv[] = {(char*)(0)};
	
	// Run constructor.
	GetOpt dut(argc, argv);
	
	// Parse 1 token- should find nothing (didn't include null).
	EXPECT_EQ(dut.getOpt(), false);
	
	// Check updated values.
	EXPECT_EQ(dut.m_asFlag, CLI_FLAG_INVALID);
	EXPECT_EQ(dut.m_type, CLI_TYPE_INVALID);
	EXPECT_EQ(dut.m_value, "");
	EXPECT_EQ(dut.m_arg, "");
}

TEST(GetOpt, EmptyStr) {
	// Input with "null" string.
	int argc = 1;
	char* empty = "";
	char* argv[] = {empty};
	
	// Run constructor.
	GetOpt dut(argc, argv);
	
	// Parse 1 token- should find nothing (didn't include empty).
	EXPECT_EQ(dut.getOpt(), false);
	
	// Check updated values.
	EXPECT_EQ(dut.m_asFlag, CLI_FLAG_INVALID);
	EXPECT_EQ(dut.m_type, CLI_TYPE_INVALID);
	EXPECT_EQ(dut.m_value, "");
	EXPECT_EQ(dut.m_arg, "");
}

TEST(GetOpt, IdHelpFlag) {
	// Input starts with "help" flag.
	int argc = 3;
	char* argv[] = {"-h", "loner", "bar"};
	
	// Run constructor.
	GetOpt dut(argc, argv);
	
	// Parse 1 token- should find something.
	EXPECT_EQ(dut.getOpt(), true);
	
	// Check updated values.
	EXPECT_EQ(dut.m_asFlag, CLI_FLAG_HELP);
}

TEST(GetOpt, IdNameFlag) {
	// Input starts with "name" flag.
	int argc = 3;
	char* argv[] = {"-o", "myArg", "bar"};
	
	// Run constructor.
	GetOpt dut(argc, argv);
	
	// Parse 1 token- should find something.
	EXPECT_EQ(dut.getOpt(), true);
	
	// Check updated values.
	EXPECT_EQ(dut.m_asFlag, CLI_FLAG_NAME);
}

TEST(GetOpt, IdBadFlag) {
	// Input starts with "name" flag.
	int argc = 3;
	char* argv[] = {"-qwertyuiop", "myArg", "bar"};
	
	// Run constructor.
	GetOpt dut(argc, argv);
	
	// Parse 1 token- should find something.
	EXPECT_EQ(dut.getOpt(), true);
	
	// Check updated values.
	EXPECT_EQ(dut.m_asFlag, CLI_FLAG_INVALID);
	EXPECT_EQ(dut.m_type, CLI_TYPE_INVALID);
	EXPECT_EQ(dut.m_value, "-qwertyuiop");
	EXPECT_EQ(dut.m_arg, "");
}