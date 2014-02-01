#include "config.h"

#include <stdio.h>
#include <gtest/gtest.h>

#include "../src/ntb-api.h"
#include "../src/ntb-util.h"

TEST(ntb_api, init)
{
        struct ntb_run_context * rc = ntb_init(NULL);
        ntb_connect(rc);
        ntb_destroy(rc);
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

