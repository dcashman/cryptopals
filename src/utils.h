#ifndef UTILS_H
#define UTILS_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_BITS 8 // using uint8

/*
 *
 */
int retrieve_bit_seq(const uint8_t *bits, size_t bits_len, size_t offset, size_t seq_len, uint8_t **seq);

/* convert ascii char to binary representation. (just returns the arg) */
char ascii_char_to_binary(char c);

/* TODO: convert to table for faster look-up */
char ascii_b64_to_binary(char c);

/* convert ascii hex char to binary representation */
char ascii_hex_to_binary(char c);

/* take an input string of ascii hex chars and convert it to its underlying binary representation */
int ascii_hex_str_to_bytes(const char *ascii_str, uint8_t **bytes, size_t *byte_size);

/* convert a binary representation from 0 - 15 to its hex char value */
char binary_to_ascii_hex(uint8_t bits);

/* convert a binary representation from 0 - 63 to its b64 char value */
char binary_to_b64(uint8_t bits);

/* convert bytes to their hex string representation */
int bytes_to_ascii_hex_str(char **ascii_str, const uint8_t *bytes, size_t bytes_size);

/* convert bytes to their b64 string representation */
int bytes_to_ascii_b64_str(char **ascii_str, const uint8_t *bytes, size_t bytes_size);

int xor_ascii(const char *r1, const char *r2, size_t len, char **out);

int hamming_weight(uint8_t b);

#ifdef __cplusplus
}
#endif
#endif /* UTILS_H */
