
#include <stdio.h>
#include <gtest/gtest.h>

#include "../config.h"
#include "../src/ntb-buffer.h"

TEST (test_ntb_buffer, is_size_correct) {
    size_t size = 32;
    struct ntb_buffer buf;
    ntb_buffer_init(&buf);
    ntb_buffer_ensure_size(&buf, size);
    ASSERT_EQ(buf.size, size); //, "size must be equals");
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
