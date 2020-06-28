#include <cstdlib>

#include "gtest/gtest.h"
#include "src/BinaryBlob.h"
#include "src/matasano_problems.h"

// TODO: Refactor this so that we're not relying on global vars defined in main.
std::string rootdir = "rootdir";
std::vector<double> english_freq_table{};

TEST(BlockTest, p11_oracle) {
    // This uses a lot of random bits, and it's deliberately opaque, so let's
    // just verify that two calls yield different results.
    BinaryBlob input = BinaryBlob("Alas poor Yorick I knew him well.", 256);
    BinaryBlob b1 = p11_oracle(input);
    BinaryBlob b2 = p11_oracle(input);
    EXPECT_NE(b1.hex(), b2.hex());
}

TEST(BlockTest, p11_detect_oracle_mode) {
    // Let's make sure we are successful a bunch, so let's do 5 runs of ecb and
    // 5 of cbc, and pass if we get 100% on all.
    for (int i = 0; i < 5; i++) {
        EXPECT_TRUE(p11_detect_oracle_mode(true /* cbc mode */));
    }
    for (int i = 0; i < 5; i++) {
        EXPECT_FALSE(p11_detect_oracle_mode(false /* ecb mode */));
    }
}

TEST(BlockTest, p12_oracle) {
    // We shoudl be using the same key, so two calls should return the same
    // result.
    BinaryBlob input = BinaryBlob("Alas poor Yorick I knew him well.", 256);
    BinaryBlob b1 = p12_oracle(input);
    BinaryBlob b2 = p12_oracle(input);
    EXPECT_EQ(b1.hex(), b2.hex());
}
