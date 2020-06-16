/*
 * Block cipher utility functions.
 */
#ifndef BLOCK_H
#define BLOCK_H

#include <limits>

#include "BinaryBlob.h"

/*
 * CBC implementation using AES as the underlying block cipher. Provided
 * plaintext will be padded prior to encryption.
 *
 * Returns the ciphertext of the provided plaintext.
 */
BinaryBlob cbc_aes_encrypt(BinaryBlob& iv, BinaryBlob plaintext, BinaryBlob& key, size_t blocksize);

/*
 * CBC implementation using AES as the underlying block cipher.
 *
 * Returns the plaintext of the provided ciphertext, with padding removed.
 */
BinaryBlob cbc_aes_decrypt(BinaryBlob& ciphertext, BinaryBlob& key, size_t blocksize);

/*
 * ECB implementation using AES as the underlying block cipher. Provided
 * plaintext will be padded prior to encryption.
 *
 * Returns the ciphertext of the provided plaintext.
 */
BinaryBlob ecb_aes_encrypt(BinaryBlob plaintext, BinaryBlob& key, size_t blocksize);

/*
 * CBC implementation using AES as the underlying block cipher.
 *
 * Returns the plaintext of the provided ciphertext, with padding removed.
 */
BinaryBlob ecb_aes_decrypt(BinaryBlob& ciphertext, BinaryBlob& key, size_t blocksize);

/*
 * Encrypt a single block of blocksize using AES.  input and key must be of
 * blocksize size.
 */
BinaryBlob aes_encrypt(BinaryBlob input, BinaryBlob key, size_t blocksize);

/*
 * Decrypt a single block of blocksize using AES.  input and key must be of
 * blocksize size.
 */
BinaryBlob aes_decrypt(BinaryBlob input, BinaryBlob key, size_t blocksize);

/*
 * Visible for testing.
 *
 * Encryption oracle specified by p11.  Takes plaintext input and encrypts under
 * a random 16-byte key, after prepending 5-10 and appending 5-10 bytes, each
 * chosen randomly.
 *
 * Arg cbc is exposed purely for testing: to verify that this does what we
 * expect.  True means use CBC, false means use ECB.
 */
BinaryBlob encryption_oracle(BinaryBlob input, bool cbc);

/*
 * Encryption oracle specified by p11.  Takes plaintext input and encrypts under
 * a random 16-byte key, after prepending 5-10 and appending 5-10 bytes, each
 * chosen randomly.
 *
 * Calls encryption_oracle(BinaryBlob, bool) with random mode.
 */
BinaryBlob encryption_oracle(BinaryBlob input);

/*
 * Takes the output of the above encryption_oracle() and determines whether ecb
 * or cbc mode was used under the hood.  This one cheats, though, and tells the
 * oracle which mode to use.  This is not used as part of the detection, but
 * instead to just verify that we've done this correctly.
 */
bool detect_oracle_mode(bool cbc);

#endif /* BLOCK_H */
