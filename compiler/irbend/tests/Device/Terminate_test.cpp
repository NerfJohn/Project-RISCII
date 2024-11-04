#include <gtest/gtest.h>
#include "Domain/RetCode_e.h"
#include "tests/Os/OsExit.h"
#include "tests/Os/OsStdout.h"

#include "Device/Terminate.h"

TEST(Terminate, silent) {
	// Execute silent terminate.
	Terminate_silent(RET_ASSERT);
	
	// Check return is correct.
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_ASSERT);
	
	// Check nothing was output.
	EXPECT_EQ(OsStdout_getOutput(), "");
}