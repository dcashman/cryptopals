#include "gtest/gtest.h"
#include "src/BinaryBlob.h"

#include <stdio.h>

class BinaryBlobTest : public ::testing::Test {
protected:
 BinaryBlob binary_blob_;

    void SetUp() override {
        uint8_t data[4] = {0, 1, 2, 3};
        binary_blob_ = BinaryBlob(data, 4);
    }
};

TEST_F(BinaryBlobTest, testPrettyB64) {
    uint8_t data[4] = {1, 2, 3, 4};
    // 1) no padding
    BinaryBlob no_padding{data, 3};
    EXPECT_EQ("AQID", no_padding.B64());
    // 2) one padded character
    BinaryBlob one_padding{data, 2};
    EXPECT_EQ("AQI=", one_padding.B64());
    // 3) two padded characters
    BinaryBlob two_padding{data, 4};
    EXPECT_EQ("AQIDBA==", two_padding.B64());
}

TEST_F(BinaryBlobTest, testBinaryAddition) {
    BinaryBlob sum = binary_blob_ + binary_blob_;
    uint8_t bytes[8];
    memcpy(bytes, binary_blob_.getRawBuf(), 4);
    memcpy(&bytes[4], binary_blob_.getRawBuf(), 4);
    EXPECT_EQ(0, memcmp(bytes, sum.getRawBuf(), sum.size()));
}

TEST_F(BinaryBlobTest, testBinaryEquality) {
    BinaryBlob zeroes = BinaryBlob('0', 4);
    BinaryBlob ones = BinaryBlob('1', 4);
    BinaryBlob ones2 = BinaryBlob('1', 4);
    EXPECT_TRUE(ones == ones2);
    EXPECT_FALSE(ones == zeroes);
}

TEST_F(BinaryBlobTest, testBinaryInequality) {
    BinaryBlob zeroes = BinaryBlob('0', 4);
    BinaryBlob ones = BinaryBlob('1', 4);
    BinaryBlob ones2 = BinaryBlob('1', 4);
    EXPECT_FALSE(ones != ones2);
    EXPECT_TRUE(ones != zeroes);
}

TEST_F(BinaryBlobTest, getByteEmpty) {
    BinaryBlob binary_blob{};
    EXPECT_EQ(0, binary_blob.getByte(0));
}

TEST_F(BinaryBlobTest, getByte) {
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(i, binary_blob_.getByte(i));
    }
}

TEST_F(BinaryBlobTest, padPKCS7) {
    binary_blob_.padPKCS7(16);
    EXPECT_EQ(16, binary_blob_.size());
    // Make sure original data wasn't altered.
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(i, binary_blob_.getByte(i));
    }
    // Make sure we added the right padding.
    for (int i = 4; i < 16; i++) {
        EXPECT_EQ(12, binary_blob_.getByte(i));
    }
}

TEST_F(BinaryBlobTest, padPKCS7Full) {
    binary_blob_.padPKCS7(4);
    EXPECT_EQ(8, binary_blob_.size());
    // Make sure original data wasn't altered.
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(i, binary_blob_.getByte(i));
    }
    // Make sure we added the right padding, in this case a whole new block.
    for (int i = 4; i < 8; i++) {
        EXPECT_EQ(4, binary_blob_.getByte(i));
    }
}

TEST_F(BinaryBlobTest, stripPKCS7) {
    // Make sure that after padding, stripping the padding will give us back
    // what we started with.  Note: Stripping padding from an unpadded blob is
    // not supported (we don't verify that it's padded first).
    binary_blob_.padPKCS7(16);
    binary_blob_.stripPKCS7();
    // Make sure original data wasn't altered.
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(i, binary_blob_.getByte(i));
    }
    // Make sure we're the right size.
    EXPECT_EQ(4, binary_blob_.size());
}

TEST_F(BinaryBlobTest, RandomBlob) {
    // Rand tests are hard, so for now let's just verify that if we create two
    // random blobs, they are distinct from each other and have the expected
    // sizes.
    BinaryBlob b1 = BinaryBlob::RandomBlob(4);
    BinaryBlob b2 = BinaryBlob::RandomBlob(4);
    EXPECT_EQ(4, b1.size());
    EXPECT_EQ(4, b2.size());
    EXPECT_NE(b1.hex(), b2.hex());
}

TEST_F(BinaryBlobTest, RandomBlobNonAligned) {
    // Also make sure that RandomBlob works for random numbers which are of
    // inconvenient sizes.
    BinaryBlob b1 = BinaryBlob::RandomBlob(7);
    BinaryBlob b2 = BinaryBlob::RandomBlob(7);
    EXPECT_EQ(7, b1.size());
    EXPECT_EQ(7, b2.size());
    EXPECT_NE(b1.hex(), b2.hex());
}
