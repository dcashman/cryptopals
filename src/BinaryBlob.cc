#include <random>

#include "BinaryBlob.h"

static const size_t BYTE_SZ = 1 << 8;

BinaryBlob::BinaryBlob() : data(0)
{
}

BinaryBlob::BinaryBlob(const uint8_t* bytes, size_t len)
{
    data.resize(len);
    for (int i = 0; i < len; i++)
        data[i] = *bytes++;
}

BinaryBlob::BinaryBlob(uint8_t byte, size_t len)
{
    while (len-- > 0)
        data.push_back(byte);
}

BinaryBlob::BinaryBlob(std::ifstream& infile)
{
    if (!infile.is_open())
        throw std::invalid_argument("Open ifstream required to read file input.");
    char c;
    while (infile.get(c))
        data.push_back((uint8_t) c);
}

BinaryBlob::BinaryBlob(const std::string s, unsigned int base)
{
    if (!(base == 16 || base == 8 || base == 2 || base == 256 || base == 64))
        throw std::invalid_argument("BinaryBlob input must supported base: [2, 8, 16, 64, 256].");
    size_t num_bits = log2(base); // # bits per input char
    size_t num_bytes = ceil(s.size() * (double) num_bits/8);
    if (num_bytes != floor(s.size() * (double) num_bits/8))
        throw std::invalid_argument("BinaryBlob not on byte boundary.");
    data.resize(num_bytes);
    char (*char_to_bin)(char in); // used for converting from different bases

    if (base == 256)
        char_to_bin = ascii_char_to_binary;
    else if (base == 64)
        char_to_bin = ascii_b64_to_binary;
    else
        char_to_bin = ascii_hex_to_binary;

    int curr_data = 0;
    int curr_bits_left = 8;
    for (int i = 0; i < s.size(); i++) {
        uint8_t curr_in = (uint8_t) char_to_bin(s[i]);

        // get bits left in (x) curr byte
        // if x > num_bits of input, just set the top x
        // otherwise:
        // save the lower (num_bits - x) bits in input2
        // shift the lower (num_bits - x) away
        // set remaining bits
        if (curr_bits_left >= num_bits) {
            setBits(data[curr_data], curr_in, curr_bits_left - num_bits, num_bits);
            curr_bits_left -= num_bits;
        } else {
            int num_saved = num_bits - curr_bits_left;
            uint8_t saved_in = getBits(curr_in, 0, num_saved);
            curr_in = curr_in >> num_saved;
            setBits(data[curr_data++], curr_in, 0, curr_bits_left);
            curr_bits_left = 8;
            setBits(data[curr_data], saved_in, curr_bits_left - num_saved, num_saved);
            curr_bits_left -= num_saved;
        }
    }

    // A base64 encoding might include padding which we need to remove, one byte
    // per padding character. Remove them here.
    if (base == 64) {
        if (*(s.end() - 1) == '=')
            data.pop_back();
        if (*(s.end() - 2) == '=')
            data.pop_back();
    }
}

uint8_t BinaryBlob::getByte(size_t n) const {
    if (n >= data.size())
        return 0;
    else
        return data[n];
}

uint8_t* BinaryBlob::getRawBuf() {
    return data.data();
}

BinaryBlob BinaryBlob::getBytesSlice(int start_index, size_t len) const {
    if (!(start_index + len <= data.size()))
        throw std::invalid_argument("Attempt to get slice beyond bounds of BinaryBlob");
    return BinaryBlob(&data[start_index], len);
}

size_t BinaryBlob::size() const
{
    return data.size();
}

BinaryBlob& BinaryBlob::operator+=(const BinaryBlob& rh)
{
    for (auto const& b : rh.data)
        this->data.push_back(b);
    return *this;
}

BinaryBlob operator+(BinaryBlob lh, const BinaryBlob& rh) {
    lh += rh;
    return lh;
}

bool operator==(const BinaryBlob& lh, const BinaryBlob& rh) {
    return lh.data == rh.data;
}

bool operator!=(const BinaryBlob& lh, const BinaryBlob& rh) {
    return !(lh.data == rh.data);
}

BinaryBlob BinaryBlob::operator^(const BinaryBlob& rh) const
{
    if (size() != rh.size()) {
        std::cerr << "BinaryBlob ^ lhs size: " << size() << " rhs size: " << rh.size() << std::endl;
        throw std::invalid_argument("BinaryBlob binary ^ operator args must have same size");
    }
    std::vector<uint8_t>::const_iterator lh_i = data.begin();
    std::vector<uint8_t>::const_iterator lh_iend = data.end();
    std::vector<uint8_t>::const_iterator rh_i = rh.data.begin();
    std::vector<uint8_t>::const_iterator rh_iend = rh.data.end();
    BinaryBlob ret;
    while (lh_i != lh_iend && rh_i != rh_iend) {
        ret.data.push_back(*lh_i++ ^ *rh_i++);
    }
    return ret;
}

std::vector<double> BinaryBlob::create_ascii_freq_table() const
{
    std::vector<int> counts(BYTE_SZ, 0);
    for (const auto& c : data)
        counts[c]++;
    unsigned int total = 0;
    for (int i = 0; i < BYTE_SZ; i++)
        total += counts[i];
    std::vector<double> res(BYTE_SZ, 0.0);
    for (int i = 0; i < BYTE_SZ; i++)
        res[i] = (double)counts[i]/total;
    return res;
}

unsigned int BinaryBlob::hammingWeight() const
{
    unsigned int res = 0;
    for (const auto& b : data)
        res += hamming_weight(b);
    return res;
}

std::string BinaryBlob::ascii() const
{
    std::string ret;
    for (int i = 0; i < size(); i++)
        ret += (char) data[i];
    return ret;
}

std::string BinaryBlob::B64() const
{
    // B64 conversion is done by making the input a multiple of 3 bytes, so we
    // may need to add some bytes to our input.  We don't want creating a B64
    // string to modify our BinaryBlob, so we create a copy which we can modify
    // here.
    std::vector<uint8_t> data_copy = data;

    // if current size is not divisible by 3, add padding
    const size_t padding_size = (3 - data_copy.size() % 3) % 3;
    for (int i = 0; i < padding_size; i++)
        data_copy.push_back(0);
    int curr = 0;
    int curr_bits_left = 8;
    std::string out;
    size_t num_bits = 6;
    while (curr != data_copy.size()) {
        if (curr_bits_left >= num_bits) {
            out += binary_to_b64(getBits(data_copy[curr], curr_bits_left - num_bits, num_bits));
            curr_bits_left -= num_bits;
            if (curr_bits_left == 0) {
                curr++;
                curr_bits_left = 8;
            }
        } else {
            // get what's left, then combine with next byte
            size_t rem_bits = num_bits - curr_bits_left;
            uint8_t tmp = getBits(data_copy[curr++], 0, curr_bits_left) << rem_bits;
            curr_bits_left = 8;


            tmp |= getBits(data_copy[curr], curr_bits_left - rem_bits, rem_bits);
            curr_bits_left -= rem_bits;
            out += binary_to_b64(tmp);
        }
    }
    // Replace the extra B64 characters generated from the padding with the
    // explicit padding character: '='.
    out.replace(out.end() - padding_size, out.end(), padding_size, '=');
    return out;
}

std::string BinaryBlob::hex() const
{
    int curr = 0;
    int curr_bits_left = 8;
    std::string out;
    size_t num_bits = 4;
    while (curr != this->size()) {
        if (curr_bits_left >= num_bits) {
            out += binary_to_ascii_hex(getBits(data[curr], curr_bits_left - num_bits, num_bits));
            curr_bits_left -= num_bits;
            if (curr_bits_left == 0) {
                curr++;
                curr_bits_left = 8;
            }
        } else {
            // get what's left, then combine with next byte
            size_t rem_bits = num_bits - curr_bits_left;
            uint8_t tmp = getBits(data[curr++], 0, curr_bits_left) << rem_bits;
            curr_bits_left = 8;


            tmp |= getBits(data[curr], curr_bits_left - rem_bits, rem_bits);
            curr_bits_left -= rem_bits;
            out += binary_to_ascii_hex(tmp);
        }
    }
    return out;
}

void BinaryBlob::padPKCS7(size_t blocksize) {
    if (blocksize > 256) {
        throw std::invalid_argument("BinaryBlob blocksize must be less than 256 (2 ^ 8).");
    }
    // We must add padding to make the binary blob a multilple of blocksize, and
    // each added padding byte should encode the number of bytes added.
    size_t padding_num = blocksize - this->size() % blocksize;
    for (int i = 0; i < padding_num; i++) {
        this->data.push_back(padding_num);
    }
    return;
}

void BinaryBlob::stripPKCS7() {
    size_t num_padding_bytes = this->data.back();
    this->data.resize(this->data.size() - num_padding_bytes);
    return;
}

/* Create a new BinaryBlob with random input for len bytes. */
BinaryBlob BinaryBlob::RandomBlob(size_t len) {
    size_t bytes_per_call = sizeof(unsigned);
    std::random_device rd{};
    BinaryBlob output{};
    while (len) {
        unsigned x = rd();
        BinaryBlob tmp = BinaryBlob((uint8_t*) &x, bytes_per_call);
        size_t copy_amount = std::min(len, bytes_per_call);
        output += tmp.getBytesSlice(0, copy_amount);
        len -= copy_amount;
    }
    return output;
}


uint8_t BinaryBlob::getBits(uint8_t b, int start, int len)
{
    return (b & ((1 << (start + len)) - 1)) >> start;
}

void BinaryBlob::setBits(uint8_t &dest, uint8_t src, int start, int len)
{
    uint8_t clear_bit_mask = ~(((1 << len) - 1) << start);
    dest &= clear_bit_mask;
    dest |=  getBits(src, 0, len) << start;
    return;
}
