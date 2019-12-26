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
