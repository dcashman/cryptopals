/*
 * sample.cpp - testing setup of gtest framework.  This should be removed and
 * replaced with real tests.
 */
#include "gtest/gtest.h"

TEST(SAMPLETEST, SAMP1) {
    EXPECT_EQ(1, 1);
    EXPECT_EQ(1, 2);
}
