
#include <stdio.h>
#include <gtest/gtest.h>

#include "../config.h"
#include "../src/ntb-base58.h"


void
base58_isomorphism(const uint8_t * input,
                   const size_t length,
                   uint8_t *output)
{
        char base58 [2*length];
        memset(base58, 0xff, 2*length);

        size_t base58_length = ntb_base58_encode(input, length, base58);
        ntb_base58_decode(base58, base58_length, output, length);
}

void
assert_is_isomorphic(const uint8_t * input,
                            const size_t length)
{
        uint8_t output[length];
        memset(output, 0xff, length);

        base58_isomorphism(input, length, output);
        for(int i = 0; i < length; i++) {
                ASSERT_EQ(input[i], output[i]);
        }
}
void assert_decoded(const char * base58,
                    uint8_t * expected,
                    size_t expected_len)
{
        size_t base58_len = strlen(base58);

        size_t hex_len = 2*strlen(base58);
        uint8_t hex[hex_len];
        size_t decoded_len = ntb_base58_decode(base58, base58_len,
                                                  hex, hex_len);

        ASSERT_EQ(expected_len, decoded_len);
        for (size_t i = 0; i < decoded_len; i++) {
                ASSERT_EQ(hex[i], expected[i]);
        }
}
TEST (test_ntb_base58, isomorphism)
{
        uint8_t zero[] = {0x00};
        uint8_t small_input[] = {0xff, 0xf0, 0x0f, 0x11, 0x0f};
        uint8_t zero_prefix[] = {0xa0, 0xff, 0xff};


        const size_t big_input_length = 512;
        uint8_t big_input[big_input_length] = {0};

        memset(big_input, 0xff, big_input_length);
        assert_is_isomorphic(zero, sizeof zero);
        assert_is_isomorphic(small_input, sizeof small_input);
        assert_is_isomorphic(zero_prefix, sizeof zero_prefix);
        assert_is_isomorphic(big_input, sizeof big_input);
}

TEST (test_ntb_base58, decoding)
{
        char zero[] = "1";  // 1 is zero in base58
        uint8_t zero_expected[] = {0x00};
        assert_decoded(zero, zero_expected, sizeof zero_expected);

        char small[] = "4A";
        uint8_t small_expected[] = {0xb7}; // 58*3 + 9 = 183= 0xb7
        assert_decoded(small, small_expected, sizeof small_expected);

        char medium[] = "23aGHzz31";
        uint8_t medium_expected[] = {0x79, 0xa4, 0x04, 0x5a, 0x7a, 0x00};
        assert_decoded(medium, medium_expected, sizeof medium_expected);

        char big[] = "1GhTfUK14zx492MNPR91rbc3";
        uint8_t big_expected[] = {0x1c, 0xcd, 0x15, 0x3a, 0x24, 0xe0, 0x6c,
                    0x5f, 0x87, 0xe6, 0xd6, 0xd6, 0x2c, 0xff, 0xcb, 0x83, 0x60};
        assert_decoded(big, big_expected, sizeof big_expected);
}
int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}
