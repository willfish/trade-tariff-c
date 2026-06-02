#include "repositories/measure_condition_repository.h"

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
    char *param = measure_condition_repository_sid_array_param(sids, sizeof(sids) / sizeof(sids[0]));

    assert_non_null(param);
    assert_string_equal(param, "{123,-456}");

    free(param);
}

static void hydrates_condition_row(void **state)
{
    (void)state;

    char *values[17] = {
        copy_literal("789"),
        copy_literal("123"),
        copy_literal("B"),
        copy_literal("1"),
        copy_literal("12.5"),
        copy_literal("EUC"),
        copy_literal("KGM"),
        NULL,
        copy_literal("C"),
        copy_literal("990"),
        copy_literal("01"),
        copy_literal("Produce certificate"),
        copy_literal("Condition text"),
        copy_literal("Certificate type"),
        copy_literal("Certificate description"),
        copy_literal("Kilogram"),
        NULL,
    };
    unsigned char nulls[17] = { 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
    DbRows rows = {
        .values = values,
        .nulls = nulls,
        .rows = 1,
        .cols = 17,
        .error = NULL,
        .ok = 1,
    };

    MeasureConditionModel model = { 0 };
    assert_true(measure_condition_repository_hydrate_row(&rows, 0, &model));
    assert_string_equal(model.measure_condition_sid, "789");
    assert_string_equal(model.measure_sid, "123");
    assert_string_equal(model.condition_code, "B");
    assert_string_equal(model.condition_duty_amount, "12.5");
    assert_string_equal(model.action, "Produce certificate");
    assert_string_equal(model.measure_condition_code_description, "Condition text");
    assert_string_equal(model.measurement_unit_description, "Kilogram");
    assert_null(model.condition_measurement_unit_qualifier_code);
    assert_null(model.measurement_unit_qualifier_description);

    measure_condition_model_free(&model);
    for (size_t i = 0; i < 17; i++) {
        test_free(values[i]);
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(builds_postgres_array_param_from_numeric_measure_sids),
        cmocka_unit_test(hydrates_condition_row),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
