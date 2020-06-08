#include "utils.h"

/* return xor of bytes.  Nothing complicated. Inputs trusted. */
void xor_bytes(const uint8_t *r1, const uint8_t *r2, size_t len, uint8_t *out) {
    for (int i = 0; i < len; i++)
        *out++ = *r1++ ^ *r2++;
}

/* xor ascii.  Assume trusted input. */
int xor_ascii(const char *r1, const char *r2, size_t len, char **out) {
  uint8_t *r1_int, *r2_int, *out_int;
  size_t r1_int_sz, r2_int_sz;

  /* convert hex to bytes */
  if (ascii_hex_str_to_bytes(r1, &r1_int, &r1_int_sz)) {
      printf("Failed to convert r1 input to byte representation.\n");
      return -1;
  }

  if (ascii_hex_str_to_bytes(r2, &r2_int, &r2_int_sz)) {
      printf("Failed to convert r2 input to byte representation.\n");
      return -1;
  }

  /* force len to be same (caller should enforce this) */
  if (r1_int_sz != r2_int_sz) {
      printf("Error: xor bytestreams differ in length!\n");
      return -1;
  }
  out_int = (uint8_t *) malloc(r1_int_sz);
  if (!out_int) {
      printf("Error: OOM.\n");
      return -1;
  }
  xor_bytes(r1_int, r2_int, r1_int_sz, out_int);
  // convert output to str
  if (bytes_to_ascii_hex_str(out, out_int, r1_int_sz)) {
      printf("Failed to convert xor'd bytes to ascii hex.\n");
      return -1;
  }
  return 0;
}

char ascii_char_to_binary(char c) {
    return c;
}

char ascii_b64_to_binary(char c) {

  // indicates 0 byte, must be dealt with in processing
  if (c == '=')
      return 0;
  else if (c == '/')
      return 63;
  else if (c == '+')
      return 62;
  else if (c >= '0' && c <= '9')
      return c - '0' + 52;
  else if (c >= 'a' && c <= 'z')
      return c - 'a' + 26;
  else if (c >= 'A' && c <= 'Z')
      return c - 'A';
  return -1;
}

char ascii_hex_to_binary(char c) {
  char zero = '0';
  char lower_a = 'a';
  char upper_a = 'A';
  if ( c >= zero && c <= zero + 9 ) {
    return c - zero;
  } else if ( c >= lower_a && c <= lower_a + 5) {
    return c - lower_a + 10;
  } else if (c >= upper_a && c <= upper_a + 5) {
    return c - upper_a + 10;
  } else {
    return -1;
  }
}

int ascii_hex_str_to_bytes(const char *ascii_str, uint8_t **bytes, size_t *byte_size) {
  size_t len;
  char tmp1, tmp2;
  uint8_t *buff;

  len = strlen(ascii_str);
  if (len == 0 || len % 2 != 0) {
    printf("Error: input ascii hex string must have some elements and end on a byte-boundary.\n");
    return -1;
  }
  buff = (uint8_t *) malloc(len / 2);
  if (buff == NULL) {
    return ENOMEM;
  }
  *byte_size = len / 2;
  for (int i = 0; i < len; i+=2) {
    if ( (tmp1 = ascii_hex_to_binary(ascii_str[i])) == -1
	 || (tmp2 = ascii_hex_to_binary(ascii_str[i+1])) == -1 ) {
      printf("Error: non-hexadecimal input character encountered in %s\n", ascii_str);
    }
    buff[i/2] = (uint8_t) ((tmp1 << 4) + tmp2);
  }
  *bytes = buff;
  return 0;
}


char binary_to_ascii_hex(uint8_t bits) {
  uint8_t mask = (1 << 4) - 1;  // fill right-most 4 bits
  if (bits & ~mask) {
    printf("hex conversion requires only 4 bits, but more were used.\n");
    return -1;
  }

  if ( bits < 10 ) {
    return '0' + bits;
  } else {
    return 'a' + bits - 10;
  }
}

char binary_to_b64(uint8_t bits) {
  uint8_t mask = (1 << 6) - 1;  // fill right-most 6 bits
  if (bits & ~mask) {
    printf("base64 conversion requires only 6 bits, but more were used.\n");
    return -1;
  }
  if (bits < 26) {
    return 'A' + bits;
  } else if (bits < 52) {
    return 'a' + bits - 26;
  } else if (bits < 62) {
    return '0' + bits - 52;
  } else if (bits == 62) {
    return '+';
  } else {
    return '/';
  }
}

int bytes_to_ascii_hex_str(char **ascii_str, const uint8_t *bytes, size_t bytes_size) {
  if (ascii_str == NULL || bytes == NULL) {
    return -1;
  }

  char *p;
  size_t ret_len = bytes_size * 2 + 1; //each byte has 2 hex characters, followed by a null terminator
  uint8_t *retrieved_bits;

  *ascii_str = (char *) malloc(ret_len);
  p = *ascii_str;
  for (int i = 0; i < ret_len-1; i++) {
    if(retrieve_bit_seq(bytes, bytes_size, i*4, 4, &retrieved_bits))
      return -1;
    *p++ = binary_to_ascii_hex(*retrieved_bits);
    free(retrieved_bits);  //allocated in retrieve_bit_seq
  }
  *p = '\0';
  return 0;
}

int bytes_to_ascii_b64_str(char **ascii_str, const uint8_t *bytes, size_t bytes_size) {
  if (ascii_str == NULL || bytes == NULL) {
    return -1;
  }
  int extra_bytes, z_padding_ct;
  size_t b64_char_count;
  char *p;
  uint8_t *padded_bytes;
  uint8_t *retrieved_bits;

  // 3 bytes fits nicely intno 4 base64. We'll deal with any additional bytes
  // thereafter specially.
  extra_bytes = bytes_size%3;

  // The extra bit-pairs added to pad out the b64 char to round the bytes out
  // to a multiple of 3.
  z_padding_ct = (3 - extra_bytes)%3;

  // Every 3 bytes will generate 4 output characters.
  b64_char_count = bytes_size/3 * 4;

  // Not sure what we're doing here...
  if(extra_bytes) {
    padded_bytes = (uint8_t *) malloc(bytes_size + z_padding_ct);
    if (!padded_bytes)
        return -ENOMEM;
    memcpy(padded_bytes, bytes, bytes_size);
    memset(padded_bytes+bytes_size, 0, z_padding_ct);
    b64_char_count += 4;  // Final 3 bytes have 4 more output characters.
  } else {
      padded_bytes = (uint8_t *) malloc(bytes_size);
      if (!padded_bytes)
          return -ENOMEM;
      memcpy(padded_bytes, bytes, bytes_size);
  }
  *ascii_str = (char *) malloc(b64_char_count + 1); // Add one for null terminator.
  p = *ascii_str;
  for (int i = 0; i < b64_char_count - z_padding_ct; i++) {
    if(retrieve_bit_seq(padded_bytes, bytes_size + z_padding_ct, i*6, 6, &retrieved_bits))
      return -1;
    *p++ = binary_to_b64(*retrieved_bits);
    free(retrieved_bits);  //allocated in retrieve_bit_seq
  }
  for(int i = 0; i < z_padding_ct; i++)
    *p++ = '=';
  *p = '\0';
  return 0;
}


/*
 * helper functions
 */
/*
 * return the bit sequence from bits starting at bit offset and of length
 * seq_len.  Must be of length <= 32, to allow simple integer output.
 * First bit is assumed to be most-significant bit.
 */
int retrieve_bit_seq(const uint8_t *bits, size_t bits_len, size_t offset, size_t seq_len, uint8_t **seq) {

  if (bits == NULL || (offset + seq_len - 1  > bits_len * NUM_BITS)) {
        printf("Error: bit sequence requested goes beyond available bits!\n");
        return -1;
    }
    if (seq_len == 0) {
        printf("Warning: bit sequence requested 0 bits, taking no action!\n");
        return 0;
    }

    uint8_t *ret_bits = (uint8_t *) malloc((seq_len-1)/NUM_BITS + 1);

    size_t starting_byte = offset/NUM_BITS;
    size_t init_byte_offset = offset % NUM_BITS;
    size_t init_byte_complement = NUM_BITS - init_byte_offset;

    int i = 0;
    int ctr = (int) seq_len;
    while (ctr > 0) {
        ret_bits[i] = bits[i + starting_byte] << init_byte_offset;
	ret_bits[i] = ret_bits[i] | ((bits[i + starting_byte + 1] >> init_byte_complement));
	if (ctr < NUM_BITS)
	  ret_bits[i] = ret_bits[i] >> (NUM_BITS - ctr);
	ctr -= NUM_BITS;
	i++;
    }
    *seq = ret_bits;
    return 0;
}

// https://en.wikipedia.org/wiki/Hamming_weight
int hamming_weight(uint8_t b) {
    uint8_t m = 0x55;
    b = (b & m)  + ((b >> 1) & m);
    m = 0x33;
    b = (b & m)  + ((b >> 2) & m);
    m = 0x0f;
    b = (b & m)  + ((b >> 4) & m);
    return b;
}
