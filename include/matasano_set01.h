#include <iostream>
#include <limits>
#include <openssl/err.h>
#include <openssl/evp.h>
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

#endif //MATASANO_SET01_H
