#include "gtest/gtest.h"
#include "src/BinaryBlob.h"

TEST(BinaryBlob, getByteEmpty) {
    BinaryBlob binaryBlob{};
    EXPECT_EQ(0, binaryBlob.getByte(0));
}

TEST(BinaryBlob, getByte) {
    // TODO: Move to setup func when used by other tests.
    uint8_t data[4] = {0, 1, 2, 3};
    BinaryBlob binaryBlob(data, 4);
    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(i, binaryBlob.getByte(i));
    }
}
