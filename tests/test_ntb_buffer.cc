
#include <stdio.h>
#include <gtest/gtest.h>

#include "../config.h"
#include "../src/ntb-buffer.h"

TEST (test_ntb_buffer, ensure_size) {
    size_t size = 32;
    struct ntb_buffer buf;
    ntb_buffer_init(&buf);

    ntb_buffer_ensure_size(&buf, size);
    ASSERT_EQ(buf.size, size);

    ntb_buffer_ensure_size(&buf, 5*size);
    ASSERT_GE(buf.size, 5*size);
}

class test_ntb_buffer_f : public testing::Test {
  protected:
    struct ntb_buffer buffer;
    struct ntb_buffer empty_buffer;
    size_t init_size;

    virtual void SetUp() {
        init_size = 32;
        ntb_buffer_init(&buffer);
        ntb_buffer_init(&empty_buffer);
        ntb_buffer_ensure_size(&buffer, init_size);
    }
};

TEST_F (test_ntb_buffer_f, ntb_set_length) {
    size_t length =  48;
    ntb_buffer_ensure_size(&buffer, init_size);
    ntb_buffer_set_length(&buffer, length);

    ASSERT_EQ(buffer.length, length);
    ASSERT_GT(buffer.size, init_size);
}

TEST_F (test_ntb_buffer_f, ntb_buffer_append) {
    size_t data_size = 16;
    uint8_t data[16] = {0};
    size_t length_before_append = buffer.length;
    ntb_buffer_append(&buffer, &data, data_size);

    ASSERT_EQ(buffer.length, length_before_append + data_size);
    ASSERT_GE(buffer.size, init_size);

    length_before_append = empty_buffer.length;
    ntb_buffer_append(&empty_buffer, &data, data_size);
    ASSERT_EQ(empty_buffer.length, length_before_append + data_size);
    ASSERT_GE(buffer.size, data_size);
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
