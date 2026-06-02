#include "repositories/quota_definition_repository.h"

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

static void builds_postgres_text_array_param_from_order_numbers(void **state)
{
    (void)state;

    const char *order_numbers[] = { "060004", "bad,value", "", "060005" };
    char *param = quota_definition_repository_text_array_param(order_numbers, sizeof(order_numbers) / sizeof(order_numbers[0]));

    assert_non_null(param);
    assert_string_equal(param, "{\"060004\",\"060005\"}");

    free(param);
}

static void hydrates_quota_definition_row(void **state)
{
    (void)state;

    char *values[21] = {
        copy_literal("888"),
        copy_literal("060004"),
        copy_literal("777"),
        copy_literal("1000.0"),
        copy_literal("900.0"),
        copy_literal("Quota description"),
        copy_literal("KGM"),
        copy_literal("GBP"),
        copy_literal("X"),
        copy_literal("2024-02-01T00:00:00.000Z"),
        copy_literal("2024-03-01"),
        copy_literal("2024-03-31"),
        copy_literal("2024-04-01"),
        copy_literal("2024-04-30"),
        copy_literal("2024-01-01T00:00:00.000Z"),
        copy_literal("2024-12-31T00:00:00.000Z"),
        copy_literal("Kilogram"),
        copy_literal("kg"),
        copy_literal("N"),
        copy_literal("N"),
        copy_literal("999"),
    };
    unsigned char nulls[21] = { 0 };
    DbRows rows = {
        .values = values,
        .nulls = nulls,
        .rows = 1,
        .cols = 21,
        .error = NULL,
        .ok = 1,
    };

    QuotaDefinitionModel model = { 0 };
    assert_true(quota_definition_repository_hydrate_row(&rows, 0, &model));
    assert_string_equal(model.quota_definition_sid, "888");
    assert_string_equal(model.quota_order_number_id, "060004");
    assert_string_equal(model.measurement_unit_description, "Kilogram");
    assert_string_equal(model.measurement_unit_abbreviation, "kg");
    assert_string_equal(model.exhausted_event_sid, "999");

    quota_definition_model_free(&model);
    for (size_t i = 0; i < 21; i++) {
        test_free(values[i]);
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(builds_postgres_text_array_param_from_order_numbers),
        cmocka_unit_test(hydrates_quota_definition_row),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
