#include "repositories/measure_footnote_repository.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

static char *copy_literal(const char *value)
{
    size_t len = strlen(value);
    char *copy = test_malloc(len + 1);
    assert_non_null(copy);
    memcpy(copy, value, len + 1);
    return copy;
}

static void builds_postgres_array_param_from_numeric_measure_sids(void **state)
{
    (void)state;

    const char *sids[] = { "123", "bad", "", "-456" };
    char *param = measure_footnote_repository_sid_array_param(sids, sizeof(sids) / sizeof(sids[0]));

    assert_non_null(param);
    assert_string_equal(param, "{123,-456}");

    free(param);
}

static void hydrates_measure_footnote_row(void **state)
{
    (void)state;

    char *values[4] = {
        copy_literal("123"),
        copy_literal("CD123"),
        copy_literal("CD123"),
        copy_literal("Footnote text"),
    };
    unsigned char nulls[4] = { 0, 0, 0, 0 };
    DbRows rows = {
        .values = values,
        .nulls = nulls,
        .rows = 1,
        .cols = 4,
        .error = NULL,
        .ok = 1,
    };

    Footnote footnote = { 0 };
    assert_true(measure_footnote_repository_hydrate_row(&rows, 0, &footnote));
    assert_string_equal(footnote.measure_sid, "123");
    assert_string_equal(footnote.id, "CD123");
    assert_string_equal(footnote.code, "CD123");
    assert_string_equal(footnote.description, "Footnote text");

    footnote_free(&footnote);
    for (size_t i = 0; i < 4; i++) {
        test_free(values[i]);
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(builds_postgres_array_param_from_numeric_measure_sids),
        cmocka_unit_test(hydrates_measure_footnote_row),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
