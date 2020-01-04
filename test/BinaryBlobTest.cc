#include "gtest/gtest.h"
#include "src/BinaryBlob.h"


class BinaryBlobTest : public ::testing::Test {
protected:
 BinaryBlob binary_blob_;

    void SetUp() override {
        uint8_t data[4] = {0, 1, 2, 3};
        binary_blob_ = BinaryBlob(data, 4);
    }
};

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
