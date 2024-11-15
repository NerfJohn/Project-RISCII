#include <gtest/gtest.h>

#include "Util/ModelUtil.h"

TEST(ModelUtil, recErr1st) {
	// Setup inputs.
	DataModel_t data;
	data.m_numErrs = 0;
	data.m_retCode = RET_SUCCESS;
	
	// Call utility under test.
	ModelUtil_recordError(data, RET_ASSERT);
	
	// Check utility update model.
	EXPECT_EQ(data.m_numErrs, 1);
	EXPECT_EQ(data.m_retCode, RET_ASSERT);
}

TEST(ModelUtil, recErr2nd) {
	// Setup inputs (as NOT 1st error).
	DataModel_t data;
	data.m_numErrs = 5;
	data.m_retCode = RET_ASSERT;
	
	// Call utility under test.
	ModelUtil_recordError(data, RET_SUCCESS);
	
	// Check utility update model.
	EXPECT_EQ(data.m_numErrs, 6);
	EXPECT_EQ(data.m_retCode, RET_SUCCESS);
}