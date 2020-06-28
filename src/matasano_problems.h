#include <iostream>
#include <limits>
#include <unordered_map>
#include <vector>

#include "Analysis.h"
#include "BinaryBlob.h"
#include "utils.h"

/* matasano_set01.h
 * author: Dan Cashman
 */
#ifndef MATASANO_SET01_H
#define MATASANO_SET01_H

extern std::vector<double> english_freq_table;
extern std::string rootdir;

std::string problem01();

std::string problem02();

std::string problem03();

std::string problem04();

std::string problem05();

std::string problem06();

std::string problem07();

std::string problem08();

std::string problem09();

std::string problem10();

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
BinaryBlob p11_oracle(BinaryBlob input, bool cbc);

/*
 * Encryption oracle specified by p11.  Takes plaintext input and encrypts under
 * a random 16-byte key, after prepending 5-10 and appending 5-10 bytes, each
 * chosen randomly.
 */
BinaryBlob p11_oracle(BinaryBlob input);

/*
 * Takes the output of the above p11_oracle() and determines whether ecb
 * or cbc mode was used under the hood.  This one cheats, though, and tells the
 * oracle which mode to use.  This is not used as part of the detection, but
 * instead to just verify that we've done this correctly.
 */
bool p11_detect_oracle_mode(bool cbc);

std::string problem11();

/*
 * An encryption oracle per specificaitons in problem 12.  Specifically, this
 * oracle uses a consistent random key to ecb encrypt the provided plaintext
 * after appending some 'secret' text to it.  The goal of the challenge is to
 * use this oracle to determine that text.
 */
BinaryBlob p12_oracle(BinaryBlob input);

std::string problem12();

#endif //MATASANO_SET01_H
