#include "Block.h"
#include "matasano_problems.h"
#include <cstdlib>

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
    instream.open(rootdir + "/res/p4.txt", std::ios::in);
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
    instream.open(rootdir + "/res/p6.txt", std::ios::in);
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
    instream.open(rootdir + "/res/p7.txt", std::ios::in);
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
    unsigned char key[] = "YELLOW SUBMARINE";
    const size_t BLOCKSIZE = 16;
    BinaryBlob output{};
    // AES_decrypt decrypts one block at a time to effectively give us ecb mode.
    for (int i = 0; i < b7.size(); i += BLOCKSIZE) {
        BinaryBlob in_blob = b7.getBytesSlice(i, BLOCKSIZE);
        BinaryBlob out_blob = aes_decrypt(in_blob, BinaryBlob(key, BLOCKSIZE), BLOCKSIZE);
        output += out_blob;
    }

    return output.ascii();
}

std::string problem08() {
    std::ifstream instream;
    instream.open(rootdir + "/res/p8.txt", std::ios::in);
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

std::string problem09() {
    BinaryBlob b9{"YELLOW SUBMARINE", 256};
    b9.padPKCS7(20);
    return b9.hex();
}

std::string problem10() {
    std::ifstream instream;
    instream.open(rootdir + "/res/p10.txt", std::ios::in);
    if (!instream.is_open()) {
        std::cerr << "Can't open p10.txt\n";
        return std::string("p10 failed.\n");
    }
    std::string in{};
    std::string line;
    while(std::getline(instream, line)) {
        in += line;
    }
    BinaryBlob b10 = BinaryBlob(in, 64);
    BinaryBlob key{"YELLOW SUBMARINE", 256};
    BinaryBlob plaintext = cbc_aes_decrypt(b10, key, 16);
    return plaintext.ascii();
}

BinaryBlob p11_oracle(BinaryBlob input, bool cbc) {
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

bool p11_detect_oracle_mode(bool cbc) {
    // To detect ECB mode, we just need to see if the oracle will encrypt the
    // blocks to the same ciphertext output. To get around the potentially 20
    // extra bytes of added noise, we just need to choose a longer message. Here
    // we choose a message which is 128 bytes long, which should be more than
    // enough.
    BinaryBlob payload{(uint8_t) 0, 128};
    BinaryBlob oracle_output = p11_oracle(payload, cbc);

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

BinaryBlob p11_oracle(BinaryBlob input) {
    return p11_oracle(input, (bool) (rand() % 2));
}

std::string problem11() {
    return "This is a placeholder.  This challenge asked to implement a function, not provide an answer.";
}

BinaryBlob p12_oracle(BinaryBlob input) {
    static BinaryBlob aes_key = BinaryBlob::RandomBlob(16);
    constexpr char SECRET[] =
        "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkg"
        "aGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBq"
        "dXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUg"
        "YnkK";
    BinaryBlob secret_text = BinaryBlob(SECRET, 64);

    return ecb_aes_encrypt(input + secret_text, aes_key, 16);
}

// Decipher the 'SECRET' included in the p12_oracle and convert to readable text.
std::string problem12() {
    // Steps to decipher secret (also in prompt).
    // 1) Since we know that the input is being padded, we can just feed bytes
    //   to the input until the ciphertext size increases.  It should increaes
    //   by one block-size, et voila.
    const BinaryBlob empty{};
    const size_t initial_size = p12_oracle(empty).size();
    size_t test_size = initial_size;
    int extra_bytes = 0;
    do {
        BinaryBlob padding = BinaryBlob('0', ++extra_bytes);

        test_size = p12_oracle(padding).size();
    } while (test_size == initial_size);

    const size_t block_size = test_size - initial_size;

    // 2)  Detect whether or not in ECB mode. This can be done by feeding the
    //   oracle 2 identical blocks and seeing if they are identical.
    BinaryBlob ecb_test = p12_oracle(BinaryBlob('0', block_size * 2));
    if (ecb_test.getBytesSlice(0, block_size) != ecb_test.getBytesSlice(block_size, block_size)) {
        return "Unable to perform attack on non-ECB oracle.";
    }

    // 3) Compute a lookup table for the values of one block where the first N
    //   bytes are always the same, and we only need to check the lookup table
    //   to determine the last BLOCKSIZE - N bytes (here just the last byte).
    BinaryBlob lookup_table[256];


    // 4) Use our ability to shift the block-alignment of the ciphertext to
    // isolate the next unknown plaintext byte at the end of a block where we
    // already know the plaintext for all of the preceding bytes in that block.
    // We bootstrap this process by filling up all but the last byte of our
    // initial input with 0's, which will form our first lookup table base.
    const size_t padding_amount = block_size - 1;
    const BinaryBlob padding_bank = BinaryBlob((uint8_t) 0, block_size - 1);

    // We will store our gradually revealed plaintext here. This will eventually
    // be used as the base of the lookup tables as we progress beyond the first
    // block, where we use our initial padding.
    BinaryBlob deduced_plaintext{};

    // 5) Generate a new lookup table and decipher one byte at a time for the
    // entire plaintext.

    for (size_t i = 0; i < initial_size; i++ ) {

        // Combine the padding with known result so far, and take the last
        // BLOCKSIZE - 1 bytes.
        BinaryBlob base = padding_bank + deduced_plaintext;
        base = base.getBytesSlice(base.size() - padding_amount, padding_amount);
        for (int j = 0; j < 256; j++) {
            BinaryBlob tmp = base;
            tmp += BinaryBlob((uint8_t) j, 1);

            // First block of oracle ciphertext is the ciphertext of our chosen
            // input.
            lookup_table[j] = p12_oracle(tmp).getBytesSlice(0, block_size);
        }

        // Get the block from the 'real result' we want to compare.
        size_t focal_block = deduced_plaintext.size() / block_size;

        // Determine how much padding we need to position the next byte at the
        // end of a block (so we can look it up in our lookup table).
        size_t padding_size =
            padding_amount - (deduced_plaintext.size() % block_size);
        BinaryBlob actual = p12_oracle(padding_bank.getBytesSlice(0, padding_size))
            .getBytesSlice(focal_block * block_size, block_size);

        // Now check our lookup table and record the latest byte.
        for (int j = 0; j < 256; j++) {
            if (lookup_table[j] == actual) {
                deduced_plaintext += BinaryBlob((uint8_t) j, 1);
                break;
            }
        }
    }
    deduced_plaintext.stripPKCS7();
    return deduced_plaintext.ascii();
}
