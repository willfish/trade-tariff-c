#include "repositories/measurement_unit_qualifier_repository.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

static void builds_postgres_text_array_for_codes(void **state)
{
    (void)state;

    const char *codes[] = { "X", NULL, "", "A" };
    char *param = measurement_unit_qualifier_repository_code_array_param(codes, sizeof(codes) / sizeof(codes[0]));

    assert_non_null(param);
    assert_string_equal(param, "{\"X\",\"A\"}");

    free(param);
}

static void hydrates_qualifier_row(void **state)
{
    (void)state;

    char *values[] = { "X", "per hectolitre" };
    unsigned char nulls[] = { 0, 0 };
    DbRows rows = {
        .ok = 1,
        .rows = 1,
        .cols = 2,
        .values = values,
        .nulls = nulls,
    };
    MeasurementUnitQualifierModel model = { 0 };

    assert_true(measurement_unit_qualifier_repository_hydrate_row(&rows, 0, &model));
    assert_string_equal(model.measurement_unit_qualifier_code, "X");
    assert_string_equal(model.description, "per hectolitre");

    measurement_unit_qualifier_model_free(&model);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(builds_postgres_text_array_for_codes),
        cmocka_unit_test(hydrates_qualifier_row),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
