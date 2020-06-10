#include "Block.h"
#include "src/include/openssl/aes.h"
#include <vector>

BinaryBlob aes_encrypt(BinaryBlob input, BinaryBlob key, size_t blocksize) {
    if (input.size() != blocksize) {
        throw std::invalid_argument("BinaryBlob input must match the blocksize");
    }
    // First let's get our key.
    AES_KEY aes_key;
    if (AES_set_encrypt_key((uint8_t *) key.ascii().c_str(), blocksize * 8, &aes_key)) {
        std::cout << "Error setting AES key with key: " << key.ascii()
                  << " and blocksize: " << blocksize << std::endl;
    }
    uint8_t out_buf[blocksize];
    AES_encrypt(input.getRawBuf(), out_buf, &aes_key);
    return BinaryBlob(out_buf, blocksize);
}

BinaryBlob aes_decrypt(BinaryBlob input, BinaryBlob key, size_t blocksize) {
    if (input.size() != blocksize) {
        throw std::invalid_argument("BinaryBlob input must match the blocksize");
    }
    // First let's get our key.
    AES_KEY aes_key;
    if (AES_set_decrypt_key((uint8_t *) key.ascii().c_str(), blocksize * 8, &aes_key)) {
        std::cout << "Error setting AES key with key: " << key.ascii()
                  << " and blocksize: " << blocksize << std::endl;
    }
    uint8_t out_buf[blocksize];
    AES_decrypt(input.getRawBuf(), out_buf, &aes_key);
    return BinaryBlob(out_buf, blocksize);
}

BinaryBlob cbc_aes_encrypt(BinaryBlob& iv, BinaryBlob plaintext, BinaryBlob& key, size_t blocksize) {
    // Partition the plaintext into blocks, xor with the previous ciphertext
    // block (or IV if first plaintext block), and then encrypt.

    // First pad to a multiple of blocksize.
    plaintext.padPKCS7(blocksize);

    // Then separate into the different blocks.
    size_t num_blocks = plaintext.size()/blocksize;
    std::cout << "DACASH plaintext num blocks after padding: " << num_blocks << std::endl;
    std::vector<BinaryBlob> input_blocks{};
    for (int i = 0; i < num_blocks; i++) {
        input_blocks.push_back(plaintext.getBytesSlice(blocksize * i, blocksize));
    }

    // Initialize the ciphertext with the initialization vector (IV).
    BinaryBlob output = iv;
    for (int i = 0; i < num_blocks; i++) {
        // First grab the last block of the ciphertext.
        BinaryBlob previous_ciphertext = output.getBytesSlice(blocksize * i, blocksize);
        std::cout << "DACASH - in[i] size: " << input_blocks[i].size() << " and prev ciphertext size: " << previous_ciphertext.size() << std::endl;
        BinaryBlob cipher_input = input_blocks[i] ^ previous_ciphertext;
        BinaryBlob ciphertext = aes_encrypt(cipher_input, key, blocksize);
        output += ciphertext;
    }

    return output;
}

BinaryBlob cbc_aes_decrypt(BinaryBlob& ciphertext, BinaryBlob& key, size_t blocksize) {
    // Reverse the previous operation:
    // 1) Decrypt each block
    // 2) xor with the previous ciphertext block.


    // Then separate into the different blocks.
    size_t num_blocks = ciphertext.size()/blocksize;
    std::vector<BinaryBlob> ciphertext_blocks{};
    for (int i = 0; i < num_blocks; i++) {
        ciphertext_blocks.push_back(ciphertext.getBytesSlice(blocksize * i, blocksize));
    }

    BinaryBlob output{};
    for (int i = 1; i < num_blocks; i++) {
        // Decrypt each block, and xor with the one before.
        std::cout << "DACASH - cb[i] size: " << ciphertext_blocks[i].size() << " and cb[i-1] size: " << ciphertext_blocks[i-1].size() << std::endl;
        BinaryBlob plaintext = aes_decrypt(ciphertext_blocks[i], key, blocksize) ^ ciphertext_blocks[i - 1];
        output += plaintext;
    }

    return output;
}
