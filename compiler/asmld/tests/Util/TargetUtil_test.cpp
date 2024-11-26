#include <gtest/gtest.h>
#include "Device/File.h"
#include "tests/Os/OsFile.h"

#include "Util/TargetUtil.h"

TEST(TargetUtil, addWord) {
	// Setup file.
	OsFile_setResponse(RET_ERR_NONE);
	File::inst().open("foobar", FILE_OP_WRITE);
	
	// Call utility.
	TargetUtil_addWord(0x0102);
	
	// Check results.
	EXPECT_EQ(OsFile_getData(), "\x01\x02");
}