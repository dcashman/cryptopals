#include "Block.h"
#include "src/include/openssl/aes.h"
#include <cstdlib>
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
    std::vector<BinaryBlob> input_blocks{};
    for (int i = 0; i < num_blocks; i++) {
        input_blocks.push_back(plaintext.getBytesSlice(blocksize * i, blocksize));
    }

    // Initialize the ciphertext with the initialization vector (IV).
    BinaryBlob output = iv;
    for (int i = 0; i < num_blocks; i++) {
        // First grab the last block of the ciphertext.
        BinaryBlob previous_ciphertext = output.getBytesSlice(blocksize * i, blocksize);
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
        BinaryBlob plaintext = aes_decrypt(ciphertext_blocks[i], key, blocksize) ^ ciphertext_blocks[i - 1];
        output += plaintext;
    }
    output.stripPKCS7();
    return output;
}

// Partition the plaintext into blocks and AES encrypt each under the key.
BinaryBlob ecb_aes_encrypt(BinaryBlob plaintext, BinaryBlob& key, size_t blocksize) {

    // First pad to a multiple of blocksize.
    plaintext.padPKCS7(blocksize);

    // Then separate into the different blocks.
    size_t num_blocks = plaintext.size()/blocksize;
    std::vector<BinaryBlob> input_blocks{};
    for (int i = 0; i < num_blocks; i++) {
        input_blocks.push_back(plaintext.getBytesSlice(blocksize * i, blocksize));
    }

    BinaryBlob output{};
    for (int i = 0; i < num_blocks; i++) {
        BinaryBlob ciphertext = aes_encrypt(input_blocks[i], key, blocksize);
        output += ciphertext;
    }

    return output;
}

// Partition the ciphertext into blocks and AES decrypt each under the key.
BinaryBlob ecb_aes_decrypt(BinaryBlob& ciphertext, BinaryBlob& key, size_t blocksize) {
    size_t num_blocks = ciphertext.size()/blocksize;
    std::vector<BinaryBlob> ciphertext_blocks{};
    for (int i = 0; i < num_blocks; i++) {
        ciphertext_blocks.push_back(ciphertext.getBytesSlice(blocksize * i, blocksize));
    }

    BinaryBlob output{};
    for (int i = 0; i < num_blocks; i++) {
        // Decrypt each block.
        BinaryBlob plaintext = aes_decrypt(ciphertext_blocks[i], key, blocksize);
        output += plaintext;
    }
    output.stripPKCS7();

    return output;
}

BinaryBlob encryption_oracle(BinaryBlob input, bool cbc) {
    const size_t BLOCKSIZE = 16;
    // 5 - 10 extra bytes added to the beginning.
    BinaryBlob modified_input = BinaryBlob::RandomBlob(rand() % 6 + 5);

    modified_input += input;

    // 5 - 10 extra bytes added to the end.
    modified_input +=  BinaryBlob::RandomBlob(rand() % 6 + 5);

    BinaryBlob aes_key = BinaryBlob::RandomBlob(BLOCKSIZE);
    BinaryBlob iv = BinaryBlob::RandomBlob(BLOCKSIZE);

    if (cbc) {
        return cbc_aes_encrypt(iv, modified_input, aes_key, BLOCKSIZE);
    } else {
        return ecb_aes_encrypt(modified_input, aes_key, BLOCKSIZE);
    }
}

BinaryBlob encryption_oracle(BinaryBlob input) {
    return encryption_oracle(input, (bool) (rand() % 2));
}


bool detect_oracle_mode(bool cbc) {
    // To detect ECB mode, we just need to see if the oracle will encrypt the
    // blocks to the same ciphertext output. To get around the potentially 20
    // extra bytes of added noise, we just need to choose a longer message. Here
    // we choose a message which is 128 bytes long, which should be more than
    // enough.
    BinaryBlob payload{(uint8_t) 0, 128};
    BinaryBlob oracle_output = encryption_oracle(payload, cbc);

    // Let's just take a slice of the output somewhere in the middle, and then
    // check to see if the same sequence is repeated.  We don't need to align on
    // a block boundary since the same pattern is repeated over and over.
    //
    // Here we start after 32 bytes, since an IV would take 16, with potentially
    // 10 more prepended, and sample 32 bytes to compare two consecutive blocks.
    BinaryBlob sample_1 = oracle_output.getBytesSlice(32, 16);
    BinaryBlob sample_2 = oracle_output.getBytesSlice(48, 16);

    // If the two blocks are identical, we've got ourselves ECB, which
    // has been arbitrarily mapped as false.
    //
    // TODO(dcashman): Fix BinaryBlob operators to support this without having
    // to convert to hex.
    return sample_1.hex() != sample_2.hex();
}
