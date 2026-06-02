#include "repositories/geographical_area_repository.h"

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
    char *param = geographical_area_repository_sid_array_param(sids, sizeof(sids) / sizeof(sids[0]));

    assert_non_null(param);
    assert_string_equal(param, "{123,-456}");

    free(param);
}

static void hydrates_geographical_area_row(void **state)
{
    (void)state;

    char *values[4] = {
        copy_literal("101"),
        copy_literal("1011"),
        copy_literal("All countries"),
        NULL,
    };
    unsigned char nulls[4] = { 0, 0, 0, 1 };
    DbRows rows = {
        .values = values,
        .nulls = nulls,
        .rows = 1,
        .cols = 4,
        .error = NULL,
        .ok = 1,
    };

    GeographicalAreaModel model = { 0 };
    assert_true(geographical_area_repository_hydrate_row(&rows, 0, &model));
    assert_string_equal(model.geographical_area_sid, "101");
    assert_string_equal(model.geographical_area_id, "1011");
    assert_string_equal(model.description, "All countries");
    assert_null(model.parent_geographical_area_sid);

    geographical_area_model_free(&model);
    for (size_t i = 0; i < 4; i++) {
        test_free(values[i]);
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(builds_postgres_array_param_from_numeric_measure_sids),
        cmocka_unit_test(hydrates_geographical_area_row),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
