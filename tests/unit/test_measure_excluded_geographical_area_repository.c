#include "repositories/measure_excluded_geographical_area_repository.h"

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
    char *param = measure_excluded_geographical_area_repository_sid_array_param(sids, sizeof(sids) / sizeof(sids[0]));

    assert_non_null(param);
    assert_string_equal(param, "{123,-456}");

    free(param);
}

static void hydrates_excluded_geographical_area_row(void **state)
{
    (void)state;

    char *values[3] = {
        copy_literal("123"),
        copy_literal("456"),
        copy_literal("AD"),
    };
    unsigned char nulls[3] = { 0, 0, 0 };
    DbRows rows = {
        .values = values,
        .nulls = nulls,
        .rows = 1,
        .cols = 3,
        .error = NULL,
        .ok = 1,
    };

    MeasureExcludedGeographicalAreaModel model = { 0 };
    assert_true(measure_excluded_geographical_area_repository_hydrate_row(&rows, 0, &model));
    assert_string_equal(model.measure_sid, "123");
    assert_string_equal(model.geographical_area_sid, "456");
    assert_string_equal(model.geographical_area_id, "AD");

    measure_excluded_geographical_area_model_free(&model);
    for (size_t i = 0; i < 3; i++) {
        test_free(values[i]);
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(builds_postgres_array_param_from_numeric_measure_sids),
        cmocka_unit_test(hydrates_excluded_geographical_area_row),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
