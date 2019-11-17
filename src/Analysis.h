/*
 * Utility functions that aid in cryptanalysis.
 */
#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "BinaryBlob.h"

/* read in binary data from text file and get the proportion of occurances of each character.
 * caller must free the table.
 */
std::vector<double> create_ascii_freq_table(const std::string& infile);

/*
 * Get diffs for each pair in two tables, square each diff, sum squares, and
 * sqrt. Lower score equates to more similar distribution.  Identical
 * distributions would return 0.
 */
double score_freq_diffs(const std::vector<double>& t1, const std::vector<double>& t2);

uint8_t extract_single_byte_xor_key(std::vector<double>& freq_table, BinaryBlob& ciphertext);

/*
 * TODO: move to more appropriate file.  Leave here for now
 */
BinaryBlob repeatingKeyXor(BinaryBlob& plaintext, std::string key);

/*
 * Break viginere cipher of sufficient length.  Assumes key length between 2 and 40.
 */
BinaryBlob breakRepeatingKeyXor(std::vector<double>& freq_table, BinaryBlob& ciphertext, size_t key_sz);

BinaryBlob breakRepeatingKeyXor(std::vector<double>& freq_table, BinaryBlob& ciphertext);

#endif /* ANALYSIS_H */
