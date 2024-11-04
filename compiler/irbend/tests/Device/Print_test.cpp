#include <gtest/gtest.h>
#include "Domain/RetCode_e.h"
#include "tests/Os/OsExit.h"
#include "tests/Os/OsStdout.h"

#include "Device/Print.h"

TEST(Print, cli) {
	// Dummy input.
	std::string input = "exact string";
	
	// Print as CLI output.
	Print::inst().cli(input);
	
	// Ensure message came through as expected.
	EXPECT_EQ(OsStdout_getOutput(), input + "\n");
}

TEST(Print, bug) {
	// Dummy input/output.
	std::string input  = "exact string";
	std::string output = "ASSERT! exact string\n";
	
	// Print as CLI output.
	Print::inst().bug(input);
	
	// Ensure message came through as expected.
	EXPECT_EQ(OsStdout_getOutput(), output);
}

TEST(Print, defaultSilent) {
	// Print as log (without setting level).
	Print::inst().log(LOG_ERROR,   "error msg");
	Print::inst().log(LOG_WARNING, "warning msg");
	Print::inst().log(LOG_INFO,    "info msg");
	Print::inst().log(LOG_DEBUG,   "deubg msg");
	
	// Ensure no messages came through.
	EXPECT_EQ(OsStdout_getOutput(), "");
}

TEST(Print, assertSilent) {
	// Set Print to no log output.
	Print::inst().setLogLevel(LOG_SILENT);
	
	// Ensure setting log level to silent isn't an assert.
	EXPECT_EQ(OsExit_hasRet(), false);
	
	// Attempt to log a "silent" message.
	Print::inst().log(LOG_SILENT, "silent msg");
	
	// Ensure assert was returned due to bad message.
	std::string output = "ASSERT! Invalid log level type used for logging\n";
	EXPECT_EQ(OsStdout_getOutput(), output);
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_ASSERT);
}

TEST(Print, levelError) {
	// Set Print to log only errors.
	Print::inst().setLogLevel(LOG_ERROR);
	
	// Log main message types.
	Print::inst().log(LOG_ERROR,   "error msg");
	Print::inst().log(LOG_WARNING, "warning msg");
	Print::inst().log(LOG_INFO,    "info msg");
	Print::inst().log(LOG_DEBUG,   "deubg msg");
	
	// Ensure assert was returned due to bad message.
	std::string output = std::string("irbend.exe [ERROR] error msg\n");
	EXPECT_EQ(OsStdout_getOutput(), output);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Print, levelWarn) {
	// Set Print to log only errors.
	Print::inst().setLogLevel(LOG_WARNING);
	
	// Log main message types.
	Print::inst().log(LOG_ERROR,   "error msg");
	Print::inst().log(LOG_WARNING, "warning msg");
	Print::inst().log(LOG_INFO,    "info msg");
	Print::inst().log(LOG_DEBUG,   "deubg msg");
	
	// Ensure assert was returned due to bad message.
	std::string output = std::string("irbend.exe [ERROR] error msg\n") +
	                                 "irbend.exe [WARN ] warning msg\n";
	EXPECT_EQ(OsStdout_getOutput(), output);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Print, levelInfo) {
	// Set Print to log only errors.
	Print::inst().setLogLevel(LOG_INFO);
	
	// Log main message types.
	Print::inst().log(LOG_ERROR,   "error msg");
	Print::inst().log(LOG_WARNING, "warning msg");
	Print::inst().log(LOG_INFO,    "info msg");
	Print::inst().log(LOG_DEBUG,   "deubg msg");
	
	// Ensure assert was returned due to bad message.
	std::string output = std::string("irbend.exe [ERROR] error msg\n")  +
	                                 "irbend.exe [WARN ] warning msg\n" +
									 "irbend.exe [INFO ] info msg\n";
	EXPECT_EQ(OsStdout_getOutput(), output);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Print, levelDebug) {
	// Set Print to log only errors.
	Print::inst().setLogLevel(LOG_DEBUG);
	
	// Log main message types.
	Print::inst().log(LOG_ERROR,   "error msg");
	Print::inst().log(LOG_WARNING, "warning msg");
	Print::inst().log(LOG_INFO,    "info msg");
	Print::inst().log(LOG_DEBUG,   "debug msg");
	
	// Ensure assert was returned due to bad message.
	std::string output = std::string("irbend.exe [ERROR] error msg\n")  +
	                                 "irbend.exe [WARN ] warning msg\n" +
									 "irbend.exe [INFO ] info msg\n"    +
									 "irbend.exe [DEBUG] debug msg\n";
	EXPECT_EQ(OsStdout_getOutput(), output);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(Print, withFile) {
	// Set Print to log only errors.
	Print::inst().setLogLevel(LOG_ERROR);
	
	// Log message with filename.
	Print::inst().log(LOG_ERROR, "foobar.c", "error msg");
	
	// Ensure assert was returned due to bad message.
	std::string output = "irbend.exe [ERROR] (foobar.c) error msg\n";
	EXPECT_EQ(OsStdout_getOutput(), output);
}

TEST(Print, withFileLine) {
	// Set Print to log only errors.
	Print::inst().setLogLevel(LOG_ERROR);
	
	// Log message with filename.
	Print::inst().log(LOG_ERROR, "foobar.c", 42, "error msg");
	
	// Ensure assert was returned due to bad message.
	std::string output = "irbend.exe [ERROR] (foobar.c/42) error msg\n";
	EXPECT_EQ(OsStdout_getOutput(), output);
}