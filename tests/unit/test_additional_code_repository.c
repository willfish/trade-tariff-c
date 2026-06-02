#include "repositories/additional_code_repository.h"

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

static void builds_postgres_array_param_from_numeric_additional_code_sids(void **state)
{
    (void)state;

    const char *sids[] = { "123", "bad", "", "456" };
    char *param = additional_code_repository_sid_array_param(sids, sizeof(sids) / sizeof(sids[0]));

    assert_non_null(param);
    assert_string_equal(param, "{123,456}");

    free(param);
}

static void hydrates_additional_code_row(void **state)
{
    (void)state;

    char *values[4] = {
        copy_literal("9001"),
        copy_literal("8"),
        copy_literal("123"),
        copy_literal("Additional duty"),
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

    AdditionalCodeModel model = { 0 };
    assert_true(additional_code_repository_hydrate_row(&rows, 0, &model));
    assert_string_equal(model.additional_code_sid, "9001");
    assert_string_equal(model.additional_code_type_id, "8");
    assert_string_equal(model.additional_code, "123");
    assert_string_equal(model.description, "Additional duty");

    additional_code_model_free(&model);
    for (size_t i = 0; i < 4; i++) {
        test_free(values[i]);
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(builds_postgres_array_param_from_numeric_additional_code_sids),
        cmocka_unit_test(hydrates_additional_code_row),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
