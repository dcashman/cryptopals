#include "gtest/gtest.h"
#include "src/utils.h"

TEST(UtilsTest, bytes_to_ascii_b64_str_no_padding) {
    uint8_t bytes[3] = { 0, 1, 2};
    char *output;
    bytes_to_ascii_b64_str(&output, bytes, 3);
    const char *expected_output = "AAEC";
    EXPECT_EQ(std::string(output), std::string(expected_output));
}

TEST(UtilsTest, bytes_to_ascii_b64_str_with_padding) {
    uint8_t bytes[2] = { 0, 1 };
    char *output;
    bytes_to_ascii_b64_str(&output, bytes, 2);
    const char *expected_output = "AAE=";
    EXPECT_EQ(std::string(output), std::string(expected_output));
}
