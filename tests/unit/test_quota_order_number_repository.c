#include "repositories/quota_order_number_repository.h"

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
    char *param = quota_order_number_repository_text_array_param(order_numbers, sizeof(order_numbers) / sizeof(order_numbers[0]));

    assert_non_null(param);
    assert_string_equal(param, "{\"060004\",\"060005\"}");

    free(param);
}

static void hydrates_quota_order_number_row(void **state)
{
    (void)state;

    char *values[4] = {
        copy_literal("777"),
        copy_literal("060004"),
        copy_literal("2024-01-01T00:00:00.000Z"),
        copy_literal("2024-12-31T00:00:00.000Z"),
    };
    unsigned char nulls[4] = { 0 };
    DbRows rows = {
        .values = values,
        .nulls = nulls,
        .rows = 1,
        .cols = 4,
        .error = NULL,
        .ok = 1,
    };

    QuotaOrderNumberModel model = { 0 };
    assert_true(quota_order_number_repository_hydrate_row(&rows, 0, &model));
    assert_string_equal(model.quota_order_number_sid, "777");
    assert_string_equal(model.quota_order_number_id, "060004");
    assert_string_equal(model.validity_start_date, "2024-01-01T00:00:00.000Z");

    quota_order_number_model_free(&model);
    for (size_t i = 0; i < 4; i++) {
        test_free(values[i]);
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(builds_postgres_text_array_param_from_order_numbers),
        cmocka_unit_test(hydrates_quota_order_number_row),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
