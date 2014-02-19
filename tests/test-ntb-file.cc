#include "config.h"

#include <gtest/gtest.h>

extern "C" {
    #include "../src/ntb-file.h"
}

static const char *FILENAME = "/tmp/notbit_test";

const char KEY[] = "3.14159265359";
const char TEXT[] = "Die Gedanken sind frei\n"
                    "wer kann sie erraten?\n"
                    "Sie fliehen vorbei\n"
                    "wie nächtliche Schatten.\n"
                    "Kein Mensch kann sie wissen,\n"
                    "kein Jäger erschießen\n"
                    "mit Pulver und Blei:\n"
                    "Die Gedanken sind frei!\n";

static void
create_empty_file(const char *filename)
{
    FILE *f = fopen(FILENAME, "w");
    fclose(f);
}
static struct ntb_file_env *
get_file_env()
{
        struct ntb_file_env *env;
        env = ntb_file_env_new((uint8_t *) KEY, strlen(KEY));
        return env;
}
TEST(ntb_file, create)
{
        struct ntb_file_env *env = get_file_env();
        struct ntb_file * file = ntb_file_create(env, FILENAME);
        ASSERT_TRUE(file != NULL);
        ntb_file_close(file);
        unlink(FILENAME);
        ntb_file_env_free(env);
}

TEST(ntb_file, open)
{
        create_empty_file(FILENAME);
        struct ntb_file_env *env = get_file_env();
        struct ntb_file * file = ntb_file_open(env, FILENAME, "w+");
        ASSERT_TRUE(file != NULL);
        ntb_file_close(file);
        ntb_file_env_free(env);
        unlink(FILENAME);
}

TEST(ntb_file, open_non_existens)
{
        struct ntb_file_env *env = get_file_env();
        struct ntb_file * file = ntb_file_open(env, "doesnotexists", "r");
        ASSERT_TRUE(file == NULL);
        ntb_file_env_free(env);
}
TEST(ntb_file, write)
{
        struct ntb_file_env *env = get_file_env();
        struct ntb_file * file = ntb_file_create(env, FILENAME);

        struct ntb_buffer write_buf;
        ntb_buffer_init(&write_buf);

        ntb_buffer_append_string(&write_buf, TEXT);
        ASSERT_TRUE(ntb_file_write(file, &write_buf));

        ASSERT_GT(ntb_file_size(file), 0);
        ntb_file_close(file);
        ntb_buffer_destroy(&write_buf);
        unlink(FILENAME);
        ntb_file_env_free(env);
}

TEST(ntb_file, write_read_isomorphism)
{
        struct ntb_file_env *env = get_file_env();
        struct ntb_file * file = ntb_file_create(env, FILENAME);

        struct ntb_buffer write_buf;
        struct ntb_buffer read_buf;
        ntb_buffer_init(&write_buf);
        ntb_buffer_init(&read_buf);

        ntb_buffer_append_string(&write_buf, TEXT);

        ASSERT_TRUE(ntb_file_write(file, &write_buf));
        ntb_file_close(file);

        file = ntb_file_open(env, FILENAME, "r");
        ASSERT_TRUE(ntb_file_read(file, &read_buf));
        ASSERT_EQ(read_buf.length, write_buf.length);
        ASSERT_EQ(memcmp(read_buf.data, write_buf.data, read_buf.length), 0);
        ntb_file_close(file);

        ntb_buffer_destroy(&write_buf);
        ntb_buffer_destroy(&read_buf);
        unlink(FILENAME);
        ntb_file_env_free(env);
}

int main(int argc, char **argv) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
}

