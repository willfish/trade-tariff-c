#include "repositories/measure_condition_component_repository.h"

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

static void builds_postgres_array_param_from_numeric_condition_sids(void **state)
{
    (void)state;

    const char *sids[] = { "789", "bad", "", "-456" };
    char *param = measure_condition_component_repository_sid_array_param(sids, sizeof(sids) / sizeof(sids[0]));

    assert_non_null(param);
    assert_string_equal(param, "{789,-456}");

    free(param);
}

static void hydrates_condition_component_row(void **state)
{
    (void)state;

    char *values[7] = {
        copy_literal("789"),
        copy_literal("01"),
        copy_literal("8.95"),
        copy_literal("EUC"),
        copy_literal("KGM"),
        NULL,
        copy_literal("Percentage duty"),
    };
    unsigned char nulls[7] = { 0, 0, 0, 0, 0, 1, 0 };
    DbRows rows = {
        .values = values,
        .nulls = nulls,
        .rows = 1,
        .cols = 7,
        .error = NULL,
        .ok = 1,
    };

    MeasureConditionComponentModel model = { 0 };
    assert_true(measure_condition_component_repository_hydrate_row(&rows, 0, &model));
    assert_string_equal(model.measure_condition_sid, "789");
    assert_string_equal(model.duty_expression_id, "01");
    assert_string_equal(model.duty_amount, "8.95");
    assert_string_equal(model.monetary_unit_code, "EUC");
    assert_string_equal(model.measurement_unit_code, "KGM");
    assert_null(model.measurement_unit_qualifier_code);
    assert_string_equal(model.duty_expression_description, "Percentage duty");

    measure_condition_component_model_free(&model);
    for (size_t i = 0; i < 7; i++) {
        test_free(values[i]);
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(builds_postgres_array_param_from_numeric_condition_sids),
        cmocka_unit_test(hydrates_condition_component_row),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
