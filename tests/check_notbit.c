
#include <stdio.h>
#include <check.h>

#include "../config.h"
#include "../src/ntb-buffer.h"

START_TEST (test_ntb_buffer)
{
    size_t size = 32;
    struct ntb_buffer buf;
    ntb_buffer_init(&buf);
    ntb_buffer_ensure_size(&buf, size);
    ck_assert_msg(buf.size == size, "size must be equals");
}
END_TEST


Suite *
buffer_suite (void)
{
    Suite *s = suite_create ("Buffer");

    /* Core test case */
    TCase *tc_core = tcase_create ("Core");
    tcase_add_test(tc_core, test_ntb_buffer);
    suite_add_tcase(s, tc_core);

    return s;
}

int
main (void)
{
    int number_failed;
    Suite *s = buffer_suite();
    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
