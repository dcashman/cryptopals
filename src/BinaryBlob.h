/*
 * BinaryBlob - class representing raw binary data
 */
#ifndef BINARY_BLOB_H
#define BINARY_BLOB_H

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "utils.h"

class BinaryBlob {
 public:
    BinaryBlob();

    /* Create binary blob holding the given binary data */
    BinaryBlob(const uint8_t* bytes, size_t len);

    /* Create binary blob which repeats given byte, len times */
    BinaryBlob(uint8_t byte, size_t len);

    /* Create binary blob from given file as pure bytes*/
    BinaryBlob(std::ifstream& infile);

    /* Create binary blob represented by the string s, with each character
     * interpreted as being from the given base.  Currently only base 16 and
     * 64 supported. */
    BinaryBlob(const std::string s, unsigned int base);

    // Get the n-indexed byte from the blob (starting at 0).
    uint8_t getByte(size_t n) const;

    // Get the raw buffer containing the underlying data of this BinaryBlob.
    //
    // WARNING: This method provides direct, unprotected access and could be
    // used to alter the BinaryBlob outside its the normal lifecylce.  Ownership
    // is not respected and there is no guarantee that this pointer will remain
    // valid.
    // TODO: Move to a proper RAII model.
    uint8_t* getRawBuf();

    // Get new binary blob representing elements [start, end)
    BinaryBlob getBytesSlice(int start_index, size_t len) const;

    /* Returns number of bytes in blob */
    size_t size() const;

    /*
     * Add two binary blobs.  concatenates the rhs to the lhs
     */
    BinaryBlob& operator+=(const BinaryBlob& rh);

    /*
     * Add two binary blobs.  concatenates the rhs to the lhs
     */
    friend BinaryBlob operator+(BinaryBlob lh, const BinaryBlob& rh);

    /*
     * Compare two blobs for equality.
     */
    friend bool operator==(const BinaryBlob& lh, const BinaryBlob& rh);

    /*
     * Compare two blobs for (in)equality.
     */
    friend bool operator!=(const BinaryBlob& lh, const BinaryBlob& rh);

    /*
     * XOR two binary blobs.  This is just the xor-ing of each byte in each.
     * sizes must be the same.
     */
    BinaryBlob operator^(const BinaryBlob &rh) const;

    /* grab frequency of each byte value, might eventually transfer this into
     * something greater than bytes. Caller must free.
     */
    std::vector<double> create_ascii_freq_table() const;

    unsigned int hammingWeight() const;

    /* pretty-printing */
    std::string ascii() const;
    std::string B64() const;
    std::string hex() const;

    /*
     * PKCS#7 padding.  Add padding to the end of the binary blob so that it is
     * a multiple of the given blocksize.  There must be padding present, so if
     * it is already a multiple of blocksize, we will have to add a whole new
     * block.  Blocksize must be less than 256, to enable the padding number to
     * be represented by one byte.  This modifies the current blob.
     */
    void padPKCS7(size_t blocksize);

    /*
     * Remove PKCS#7 padding.
     */
    void stripPKCS7();

    /* Create a new BinaryBlob with random input for len bytes.
     *
     * CAUTION: This is not cryptographically secure, despite its use as part of
     *  a crytpo challenge
     */
    static BinaryBlob RandomBlob(size_t len);


 private:
    static uint8_t getBits(uint8_t b, int start, int len);
    static void setBits(uint8_t &dest, uint8_t src, int start, int len);
    std::vector<uint8_t> data;
};

#endif /* BINARY_BLOB_H */
