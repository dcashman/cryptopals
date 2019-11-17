#include "Analysis.h"

static const size_t BYTE_SZ = 256;

std::vector<double> create_ascii_freq_table(const std::string& infile)
{
    std::ifstream instream;
    instream.open(infile, std::ios::in | std::ios::binary);
    if (!instream.is_open()) {
        std::cerr << "Unable to open " << infile << " err: " << strerror(errno) << "\n";
        throw std::invalid_argument("Can't open file.");
    }
    char c;
    unsigned long long total = 0;
    std::vector<int> counts(BYTE_SZ, 0);
    while (instream.get(c))
        counts[c]++;

    for (int i = 0; i < BYTE_SZ; i++)
        total += counts[i];

    std::vector<double> res(BYTE_SZ, 0.0);
    for (int i = 0; i < BYTE_SZ; i++)
        res[i] = counts[i]/(double)total;
    return res;
}

double score_freq_diffs(const std::vector<double>& t1, const std::vector<double>& t2)
{
    if (t1.size() != t2.size())
        throw std::invalid_argument("Frequency tables must contain the same number of elements.");
    double res = 0;
    for (int i = 0; i < t1.size(); i++) {
        res += std::pow(t1[i] - t2[i], 2);
    }
    return std::sqrt(res);
}

uint8_t extract_single_byte_xor_key(std::vector<double>& freq_table, BinaryBlob& ciphertext) {
    // iterate over all values, just as check
    int mindex = -1;
    double min_score = std::numeric_limits<double>::max();
    BinaryBlob svd;
    for (int i = 0; i < 256; i++) {
        BinaryBlob testKey(i, ciphertext.size());
        std::vector<double> freqs = (ciphertext ^ testKey).create_ascii_freq_table();
        double tmp = score_freq_diffs(freq_table, freqs);
        if (tmp <= min_score) {
            mindex = i;
            min_score = tmp;
            svd = testKey;
        }
    }
    /* all bytes are the same, just return the first */
    return svd.getByte(0);
}

BinaryBlob repeatingKeyXor(BinaryBlob& plaintext, std::string key)
{
    size_t len = key.length();
    size_t full_len = plaintext.size();
    int r = full_len % len;
    int q = full_len/len;
    std::string built(key);

    // copy string to match input blob length, going over if necessary
    for (int i = 0; i < q; i++)
        built += key;
    // remove any excess characters copied over
    for (int i = 0; i < len - r; i++)
        built.pop_back();

    // xor with original
    return (BinaryBlob(built, 256) ^ plaintext);

}

BinaryBlob breakRepeatingKeyXor(std::vector<double>& freq_table, BinaryBlob& ciphertext, size_t key_sz)
{
    // create a new ciphertext corresponding to each key in key_sz
    std::vector<BinaryBlob> ciphertext_split(key_sz);
    for (int i = 0; i < ciphertext.size(); i++) {
        BinaryBlob tmp = BinaryBlob(ciphertext.getByte(i), 1);
        ciphertext_split.at(i % key_sz) += tmp;
    }

    /* get key */
    BinaryBlob key = BinaryBlob();
    for (auto& b : ciphertext_split) {
        BinaryBlob tmp = BinaryBlob(extract_single_byte_xor_key(freq_table, b), 1);
        key += tmp;
    }
    return repeatingKeyXor(ciphertext, std::string(key.ascii()));
}

/*
 * Break viginere cipher of sufficient length.  Assumes key length between 2 and 40.
 */
 BinaryBlob breakRepeatingKeyXor(std::vector<double>& freq_table, BinaryBlob& ciphertext)
{
    // determine likely key-size: take first 4 key-size chunks and compute
    // average hamming distance between them and take minimum.
    const size_t NUM_BLOCKS = 4;
    double min_score = std::numeric_limits<double>::max();
    int min_ks = 0;
    for (int ks = 1; ks <= 40; ks++) {
        std::vector<BinaryBlob> sliceVec{};
        for (int i = 0; i < NUM_BLOCKS; i++) {
            sliceVec.push_back(ciphertext.getBytesSlice(i * ks, ks));
        }
        std::vector<double> hammingScores{};
        for (int i = 0; i < NUM_BLOCKS; i++) {
            for (int j = 1; j < NUM_BLOCKS; j++) {
                if (i == j)
                    continue;
                hammingScores.push_back((sliceVec[i] ^ sliceVec[j]).hammingWeight()/ks);
            }
        }
        double total = 0;
        for (auto d : hammingScores)
            total += d;
        if (total/hammingScores.size() < min_score) {
            min_ks = ks;
            min_score = total/hammingScores.size();
        }
    }
    std::cout << "DACASH - keysize is probably " << min_ks << "\n";
    return breakRepeatingKeyXor(freq_table, ciphertext, min_ks);
}
