#include <gtest/gtest.h>
#include "Device/Print.h"
#include "Domain/RetCode_e.h"
#include "tests/Os/OsExit.h"
#include "tests/Os/OsStdout.h"

#include "Device/Terminate.h"

TEST(Terminate, silent) {
	// Execute silent terminate.
	Terminate_silent(RET_SUCCESS);
	
	// Check return is correct.
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_SUCCESS);
	
	// Check nothing was output.
	EXPECT_EQ(OsStdout_getOutput(), "");
}

TEST(Terminate, terminateAssert) {
	// Execute asserting terminate.
	Terminate_assert("assert message");
	
	// Check return is correct.
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_ASSERT);
	
	// Check assert was output.
	EXPECT_EQ(OsStdout_getOutput(), "ASSERT! assert message\n");
}

TEST(Terminate, terminateSummary) {
	// Setup inputs.
	DataModel_t data;
	data.m_numErrs = 3;
	data.m_retCode = RET_NO_READ;
	Print::inst().setLogLevel(LOG_INFO);
	
	// Execute summary terminate.
	Terminate_summary(data);
	
	// Check return is correct.
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_NO_READ);
	
	// Check assert was output.
	EXPECT_EQ(OsStdout_getOutput(), "asmld.exe [INFO ] 3 error(s), returned 2\n");
}