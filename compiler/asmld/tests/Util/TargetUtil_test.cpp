#include <gtest/gtest.h>
#include "Device/File.h"
#include "tests/Os/OsFile.h"

#include "Util/TargetUtil.h"

TEST(TargetUtil, addTextHeader) {
	// Setup file.
	OsFile_setResponse(RET_ERR_NONE);
	File::inst().open("foobar", FILE_OP_WRITE);
	
	// Call utility.
	TargetUtil_addTextHeader(8);
	
	// Check results.
	std::string exp = "";
	exp += (char)(0x00);
	exp += (char)(0x03);
	EXPECT_EQ(OsFile_getData(), exp);
}

TEST(TargetUtil, addDataHeader) {
	// Setup file.
	OsFile_setResponse(RET_ERR_NONE);
	File::inst().open("foobar", FILE_OP_WRITE);
	
	// Call utility.
	TargetUtil_addDataHeader(2);
	
	// Check results.
	std::string exp = "";
	exp += (char)(0x80);
	exp += (char)(0x00);
	EXPECT_EQ(OsFile_getData(), exp);
}

TEST(TargetUtil, addWord) {
	// Setup file.
	OsFile_setResponse(RET_ERR_NONE);
	File::inst().open("foobar", FILE_OP_WRITE);
	
	// Call utility.
	TargetUtil_addWord(0x0102);
	
	// Check results.
	std::string exp = "";
	exp += (char)(0x01);
	exp += (char)(0x02);
	EXPECT_EQ(OsFile_getData(), exp);
}