#include "matasano_set01.h"

/* convert hex to b64 */
std::string problem01() {
    BinaryBlob b1("49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d", 16);
    return b1.B64();
}

/* fixed xor: xor two equal-length buffers together and print output */
std::string problem02() {
    BinaryBlob b2l("1c0111001f010100061a024b53535009181c", 16);
    BinaryBlob b2r("686974207468652062756c6c277320657965", 16);
    BinaryBlob b2 = b2l ^ b2r;
    return b2.hex();
}

/* single-byte xor cipher: decrypt hex ciphertext encrypted with single byte xor */
std::string problem03() {
    BinaryBlob b3in("1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736", 16);
    uint8_t key = extract_single_byte_xor_key(english_freq_table, b3in);
    BinaryBlob tmp = BinaryBlob(key, b3in.size());
    return (b3in ^ tmp).ascii(); // get decrypted message
}

/* find string encrypted w/single byte xor cipher */
std::string problem04() {
    //p4 - read strings from file and then score each one. Only one should have been xor'd, so it should have the highest
    // score (least english-like).  Actually it looks like they're all garbage except for that one.  xor all strings w/all keys
    // and grab lowest scoring.
    std::ifstream instream;
    instream.open(rootdir + "res/p4.txt", std::ios::in);
    if (!instream.is_open()) {
        std::cerr << "Can't open p4.txt\n";
        return std::string("p04 failed.\n");
    }

    int count = 0;
    std::string line;
    double min_score = std::numeric_limits<double>::max();
    int mindex = 0;
    BinaryBlob svd;
    BinaryBlob svd_key;
    while(std::getline(instream, line)) {
        BinaryBlob b4in(line, 16);
        for (int i = 0; i < 256; i++) {
            BinaryBlob b4key(i, b4in.size());
            std::vector<double> freqs = (b4in ^ b4key).create_ascii_freq_table();
            double tmp = score_freq_diffs(english_freq_table, freqs);
            if (tmp <= min_score) {
                mindex = count;
                min_score = tmp;
                svd_key = b4key;
                svd = b4in;
            }
        }
        count++;
    }
    return std::string("p4: input str # " + std::to_string(mindex) + " key: " + svd_key.ascii() + " output: " + (svd ^ svd_key).ascii() + "\n");
}

/* implement repeating key xor */
std::string problem05() {
    BinaryBlob b5in("Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal", 256);
    BinaryBlob b5 = repeatingKeyXor(b5in,"ICE");
    return b5.hex();
}

/* decrypt repeating key xor */
std::string problem06() {
    std::ifstream instream;
    instream.open(rootdir + "res/p6.txt", std::ios::in);
    if (!instream.is_open()) {
        std::cerr << "Can't open p6.txt\n";
        return std::string("p06 failed.\n");
    }
    std::string in{};
    std::string line;
    while(std::getline(instream, line)) {
        in += line;
    }
    BinaryBlob b6 = BinaryBlob(in, 64);
    BinaryBlob s6 = breakRepeatingKeyXor(english_freq_table, b6);
    return s6.ascii();
}

std::string problem07() {
    std::ifstream instream;
    instream.open(rootdir + "res/p7.txt", std::ios::in);
    if (!instream.is_open()) {
        std::cerr << "Can't open p7.txt\n";
        return std::string("p07 failed.\n");
    }
    std::string in{};
    std::string line;
    while(std::getline(instream, line)) {
        in += line;
    }
    BinaryBlob b7 = BinaryBlob(in, 64);
    int num_bytes = b7.size();
    int act_num_bytes = 0, tot_bytes = 0;
    unsigned char key[] = "YELLOW SUBMARINE";
    EVP_CIPHER_CTX* my_cipher_ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(my_cipher_ctx, EVP_aes_128_ecb(), NULL, key, NULL);
    unsigned char* out = (unsigned char *) malloc(num_bytes * 2 + EVP_CIPHER_CTX_block_size(my_cipher_ctx));
    if (!out)
        return  "p7 - OOM\n";
    EVP_DecryptUpdate(my_cipher_ctx, out, &act_num_bytes, b7.getRawBuf(), num_bytes);
    tot_bytes += act_num_bytes;
    EVP_DecryptFinal_ex(my_cipher_ctx, out + act_num_bytes, &act_num_bytes);
    tot_bytes += act_num_bytes;
    EVP_CIPHER_CTX_free(my_cipher_ctx);
    BinaryBlob s7 = BinaryBlob(out, tot_bytes);
    return s7.ascii();
}

std::string problem08() {
    std::ifstream instream;
    instream.open(rootdir + "res/p8.txt", std::ios::in);
    if (!instream.is_open()) {
        std::cerr << "Can't open p8.txt\n";
        return std::string("p08 failed.\n");
    }
    std::string line;
    int max_collisions = 0;
    BinaryBlob max;
    while(std::getline(instream, line)) {
	BinaryBlob lineBlob = BinaryBlob(line, 16);
	// map bytes as string for convenience
	std::unordered_map<std::string, int> cols{};
	int tmp_cols = 0;
	// go through each block in blob, except for last if less than full
	for (int i = 0; i <= lineBlob.size() - 16; i+=16) {
	    BinaryBlob tmp = lineBlob.getBytesSlice(i, 16);
	    if (cols.count(tmp.ascii()))
	        tmp_cols++;
	    else
	        cols.insert({tmp.ascii(), 1});
	}
	if (tmp_cols >= max_collisions) {
	    max_collisions = tmp_cols;
	    max = lineBlob;
	}
    }
    return max.hex();
}
