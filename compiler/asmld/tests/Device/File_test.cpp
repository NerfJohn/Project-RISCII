#include <gtest/gtest.h>
#include "Domain/RetCode_e.h"
#include "tests/Os/OsExit.h"
#include "tests/Os/OsFile.h"
#include "tests/Os/OsStdout.h"

#include "Device/File.h"

TEST(File, openReadGood) {
	// Open "good" read file.
	OsFile_setResponse(RET_ERR_NONE);
	RetErr_e res = File::inst().open("goodFile", FILE_OP_READ);
	
	// Check operation went well.
	EXPECT_EQ(res, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(File, openWriteGood) {
	// Open "good" write file.
	OsFile_setResponse(RET_ERR_NONE);
	RetErr_e res = File::inst().open("goodFile", FILE_OP_WRITE);
	
	// Check operation went well.
	EXPECT_EQ(res, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(File, openReadBad) {
	// Open "bad" read file.
	OsFile_setResponse(RET_ERR_ERROR);
	RetErr_e res = File::inst().open("badFile", FILE_OP_READ);
	
	// Check operation failed, but passed to caller.
	EXPECT_EQ(res, RET_ERR_ERROR);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(File, openWriteBad) {
	// Open "bad" write file.
	OsFile_setResponse(RET_ERR_ERROR);
	RetErr_e res = File::inst().open("badFile", FILE_OP_WRITE);
	
	// Check operation failed, but passed to caller.
	EXPECT_EQ(res, RET_ERR_ERROR);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(File, openAlreadyOpen) {
	// Open a file.
	OsFile_setResponse(RET_ERR_NONE);
	File::inst().open("file1", FILE_OP_READ);
	
	// Open a 2nd file prior to closing.
	File::inst().open("file2", FILE_OP_READ);
	
	// Check operation resulted in assert.
	std::string output = "ASSERT! Opened 'file2' before closing 'file1'\n";
	EXPECT_EQ(OsStdout_getOutput(), output);
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_ASSERT);
}

TEST(File, openCloseOpen) {
	// Open a file.
	OsFile_setResponse(RET_ERR_NONE);
	File::inst().open("file1", FILE_OP_READ);
	
	// Close file.
	File::inst().close();
	
	// Open a 2nd file prior to closing.
	RetErr_e res = File::inst().open("file2", FILE_OP_READ);
	
	// Check operation went well.
	EXPECT_EQ(res, RET_ERR_NONE);
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(File, getNameGood) {
	// Open "good" read file.
	OsFile_setResponse(RET_ERR_NONE);
	File::inst().open("goodFile", FILE_OP_READ);
	
	// Get the name.
	std::string name = File::inst().getName();
	
	// Check name is correct.
	EXPECT_EQ(name, "goodFile");
}

TEST(File, getNameBad) {
	// Get the name when no file is open.
	std::string name = File::inst().getName();
	
	// Check operation resulted in assert.
	std::string output = "ASSERT! Fetched filename without opening file\n";
	EXPECT_EQ(OsStdout_getOutput(), output);
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_ASSERT);
}

TEST(File, peekGood) {
	// Open "good" read file.
	OsFile_setResponse(RET_ERR_NONE);
	OsFile_setFile("goodFile", "dataHere");
	RetErr_e res = File::inst().open("goodFile", FILE_OP_READ);
	
	// Peek byte from file.
	uint8_t byte = File::inst().peek();
	
	// Check operation went well.
	EXPECT_EQ(byte, (uint8_t)('d'));
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(File, peekWrite) {
	// Open "good" write file.
	OsFile_setResponse(RET_ERR_NONE);
	OsFile_setFile("goodFile", "dataHere");
	RetErr_e res = File::inst().open("goodFile", FILE_OP_WRITE);
	
	// Peek byte from file.
	uint8_t byte = File::inst().peek();
	
	// Check operation resulted in assert.
	std::string output = "ASSERT! Peeked write file 'goodFile'\n";
	EXPECT_EQ(OsStdout_getOutput(), output);
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_ASSERT);
}

TEST(File, peekNoFile) {
	// Peek byte from file.
	uint8_t byte = File::inst().peek();
	
	// Check operation resulted in assert.
	std::string output = "ASSERT! Peeked without opening file\n";
	EXPECT_EQ(OsStdout_getOutput(), output);
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_ASSERT);
}

TEST(File, peekGone) {
	// Open "good" read file.
	OsFile_setResponse(RET_ERR_NONE);
	OsFile_setFile("goodFile", "dataHere");
	RetErr_e res = File::inst().open("goodFile", FILE_OP_READ);
	
	// Ensure next OsFile operation will fail.
	OsFile_setResponse(RET_ERR_ERROR);
	
	// Peek byte from file.
	uint8_t byte = File::inst().peek();
	
	// Check operation resulted in assert.
	std::string output = "ASSERT! Unable to peek file 'goodFile'\n";
	EXPECT_EQ(OsStdout_getOutput(), output);
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_ASSERT);
}

TEST(File, popGood) {
	// Open "good" read file.
	OsFile_setResponse(RET_ERR_NONE);
	OsFile_setFile("goodFile", "dataHere");
	RetErr_e res = File::inst().open("goodFile", FILE_OP_READ);
	
	// Peek byte from file.
	uint8_t byte = File::inst().pop();
	
	// Check operation went well.
	EXPECT_EQ(byte, (uint8_t)('d'));
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(File, popWrite) {
	// Open "good" write file.
	OsFile_setResponse(RET_ERR_NONE);
	OsFile_setFile("goodFile", "dataHere");
	RetErr_e res = File::inst().open("goodFile", FILE_OP_WRITE);
	
	// Peek byte from file.
	uint8_t byte = File::inst().pop();
	
	// Check operation resulted in assert.
	std::string output = "ASSERT! Popped write file 'goodFile'\n";
	EXPECT_EQ(OsStdout_getOutput(), output);
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_ASSERT);
}

TEST(File, popNoFile) {
	// Peek byte from file.
	uint8_t byte = File::inst().pop();
	
	// Check operation resulted in assert.
	std::string output = "ASSERT! Popped without opening file\n";
	EXPECT_EQ(OsStdout_getOutput(), output);
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_ASSERT);
}

TEST(File, popGone) {
	// Open "good" read file.
	OsFile_setResponse(RET_ERR_NONE);
	OsFile_setFile("goodFile", "dataHere");
	RetErr_e res = File::inst().open("goodFile", FILE_OP_READ);
	
	// Ensure next OsFile operation will fail.
	OsFile_setResponse(RET_ERR_ERROR);
	
	// Peek byte from file.
	uint8_t byte = File::inst().pop();
	
	// Check operation resulted in assert.
	std::string output = "ASSERT! Unable to pop file 'goodFile'\n";
	EXPECT_EQ(OsStdout_getOutput(), output);
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_ASSERT);
}

TEST(File, writeGoodByte) {
	// Open "good" read file.
	OsFile_setResponse(RET_ERR_NONE);
	OsFile_setFile("goodFile", "dataHere");
	RetErr_e res = File::inst().open("goodFile", FILE_OP_WRITE);
	
	// Peek byte from file.
	File::inst().write(0x34);
	
	// Check operation went well.
	EXPECT_EQ(OsFile_getData(), "4");
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(File, writeGoodString) {
	// Open "good" read file.
	OsFile_setResponse(RET_ERR_NONE);
	OsFile_setFile("goodFile", "dataHere");
	RetErr_e res = File::inst().open("goodFile", FILE_OP_WRITE);
	
	// Peek byte from file.
	File::inst().write("a string");
	
	// Check operation went well.
	EXPECT_EQ(OsFile_getData(), "a string");
	EXPECT_EQ(OsExit_hasRet(), false);
}

TEST(File, writeRead) {
	// Open "good" write file.
	OsFile_setResponse(RET_ERR_NONE);
	OsFile_setFile("goodFile", "dataHere");
	RetErr_e res = File::inst().open("goodFile", FILE_OP_READ);
	
	// Peek byte from file.
	File::inst().write(0x34);
	
	// Check operation resulted in assert.
	std::string output = "ASSERT! Wrote read file 'goodFile'\n";
	EXPECT_EQ(OsStdout_getOutput(), output);
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_ASSERT);
}

TEST(File, writeNoFile) {
	// Peek byte from file.
	File::inst().write(0x34);
	
	// Check operation resulted in assert.
	std::string output = "ASSERT! Wrote without opening file\n";
	EXPECT_EQ(OsStdout_getOutput(), output);
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_ASSERT);
}

TEST(File, writeGone) {
	// Open "good" read file.
	OsFile_setResponse(RET_ERR_NONE);
	OsFile_setFile("goodFile", "dataHere");
	RetErr_e res = File::inst().open("goodFile", FILE_OP_WRITE);
	
	// Ensure next OsFile operation will fail.
	OsFile_setResponse(RET_ERR_ERROR);
	
	// Peek byte from file.
	File::inst().write(0x34);
	
	// Check operation resulted in assert.
	std::string output = "ASSERT! Unable to write file 'goodFile'\n";
	EXPECT_EQ(OsStdout_getOutput(), output);
	EXPECT_EQ(OsExit_hasRet(), true);
	EXPECT_EQ(OsExit_getCode(), RET_ASSERT);
}