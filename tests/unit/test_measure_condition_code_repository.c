#include "repositories/measure_condition_code_repository.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

static void builds_postgres_text_array_for_codes(void **state)
{
    (void)state;

    const char *codes[] = { "B", NULL, "", "V" };
    char *param = measure_condition_code_repository_code_array_param(codes, sizeof(codes) / sizeof(codes[0]));

    assert_non_null(param);
    assert_string_equal(param, "{\"B\",\"V\"}");

    free(param);
}

static void hydrates_condition_code_row(void **state)
{
    (void)state;

    char *values[] = {
        "B",
        "Presentation of a certificate/licence/document",
        "1970-01-01 00:00:00",
        NULL,
    };
    unsigned char nulls[] = { 0, 0, 0, 1 };
    DbRows rows = {
        .ok = 1,
        .rows = 1,
        .cols = 4,
        .values = values,
        .nulls = nulls,
    };
    MeasureConditionCodeModel model = { 0 };

    assert_true(measure_condition_code_repository_hydrate_row(&rows, 0, &model));
    assert_string_equal(model.condition_code, "B");
    assert_string_equal(model.description, "Presentation of a certificate/licence/document");
    assert_string_equal(model.validity_start_date, "1970-01-01 00:00:00");
    assert_null(model.validity_end_date);

    measure_condition_code_model_free(&model);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(builds_postgres_text_array_for_codes),
        cmocka_unit_test(hydrates_condition_code_row),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
