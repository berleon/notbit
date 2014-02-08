#include "config.h"

#include <stdio.h>
#include <gtest/gtest.h>

extern "C" {
#include "../src/ntb-api.h"
#include "../src/ntb-store.h"
#include "../src/ntb-util.h"
}

static bool created = false;
static pthread_t main_thread;
static pthread_mutex_t on_create_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t on_create_cond;

static void
on_create(struct ntb_key *key,
          void *user_data)
{
        created = true;

        pthread_mutex_lock(&on_create_mutex);
        pthread_cond_signal(&on_create_cond);
        pthread_mutex_unlock(&on_create_mutex);
}

TEST(ntb_api, DISABLED_init)
{
        struct ntb_run_context * rc = ntb_init(NULL);
        ntb_connect(rc);
        ntb_destroy(rc);
}
TEST(ntb_api, create_key)
{
        main_thread = pthread_self();
        pthread_cond_init(&on_create_cond, NULL);
        pthread_mutex_lock(&on_create_mutex);
        struct ntb_run_config *conf = ntb_run_config_default();
        struct ntb_run_context *rc = ntb_init(conf);
        const int leading_zeroes = 0;
        ntb_create_key(rc, "test-key", leading_zeroes, on_create, rc);
        pthread_cond_wait(&on_create_cond, &on_create_mutex);
        ntb_destroy(rc);
        ntb_run_config_free(rc->config);
        ASSERT_TRUE(created);
        pthread_mutex_unlock(&on_create_mutex);
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

