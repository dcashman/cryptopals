#include "gtest/gtest.h"
#include "src/BinaryBlob.h"
#include "src/Block.h"

TEST(BlockTest, aes_encrypt) {
    // We're going to treat AES as a black box, so let's just make sure that an
    // encryption decrypts back to itself.
    // We'll cheat by using the p07 example as a 'known good' conversion rather
    // than messing with openssl params.
    BinaryBlob input = BinaryBlob("I'm back and I'm", 256);
    BinaryBlob key = BinaryBlob("YELLOW SUBMARINE", 256);
    BinaryBlob output = aes_encrypt(input, key, 16);
    EXPECT_EQ(output.size(), 16);
    EXPECT_EQ(output.B64(), "CRIwqt4+szDbqkNY+I0qbAAA==");
}

TEST(BlockTest, aes_decrypt) {
    // We've already verified that aes_encrypt works as above, so here we just
    // make sure that if we encrypt something and then decrypt it, we get the
    // original plaintext back.
    BinaryBlob input = BinaryBlob("I'm back and I'm", 256);
    BinaryBlob key = BinaryBlob("YELLOW SUBMARINE", 256);
    BinaryBlob output = aes_encrypt(input, key, 16);
    BinaryBlob original = aes_decrypt(output, key, 16);
    EXPECT_EQ(input.hex(), original.hex());
}

TEST(BlockTest, cbc_aes_decrypt) {
    // We've already verified that cbc_aes_encrypt works as above, so here we just
    // make sure that if we encrypt something and then decrypt it, we get the
    // original plaintext back.
    BinaryBlob iv = BinaryBlob("initializationV!", 256);
    BinaryBlob input = BinaryBlob("I'm back and I'm", 256);
    BinaryBlob key = BinaryBlob("YELLOW SUBMARINE", 256);
    BinaryBlob output = cbc_aes_encrypt(iv, input, key, 16);
    BinaryBlob original = cbc_aes_decrypt(output, key, 16);
    original.stripPKCS7();
    EXPECT_EQ(input.hex(), original.hex());
}
