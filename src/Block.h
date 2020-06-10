/*
 * Block cipher utility functions.
 */
#ifndef BLOCK_H
#define BLOCK_H

#include <limits>

#include "BinaryBlob.h"

/*
 * CBC implementation using AES as the underlying block cipher.
 *
 * Returns the ciphertext of the provided plaintext.
 */
BinaryBlob cbc_aes_encrypt(BinaryBlob& iv, BinaryBlob plaintext, BinaryBlob& key, size_t blocksize);

/*
 * CBC implementation using AES as the underlying block cipher.
 *
 * Returns the plaintext of the provided ciphertext.
 */
BinaryBlob cbc_aes_decrypt(BinaryBlob& ciphertext, BinaryBlob& key, size_t blocksize);

BinaryBlob aes_encrypt(BinaryBlob input, BinaryBlob key, size_t blocksize);

BinaryBlob aes_decrypt(BinaryBlob input, BinaryBlob key, size_t blocksize);

#endif /* BLOCK_H */
