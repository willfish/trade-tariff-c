#include "repositories/measurement_unit_repository.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

static void builds_postgres_text_array_for_codes(void **state)
{
    (void)state;

    const char *codes[] = { "KGM", NULL, "", "ASV" };
    char *param = measurement_unit_repository_code_array_param(codes, sizeof(codes) / sizeof(codes[0]));

    assert_non_null(param);
    assert_string_equal(param, "{\"KGM\",\"ASV\"}");

    free(param);
}

static void hydrates_unit_row(void **state)
{
    (void)state;

    char *values[] = { "KGM", "Kilogram", "kg" };
    unsigned char nulls[] = { 0, 0, 0 };
    DbRows rows = {
        .ok = 1,
        .rows = 1,
        .cols = 3,
        .values = values,
        .nulls = nulls,
    };
    MeasurementUnitModel model = { 0 };

    assert_true(measurement_unit_repository_hydrate_row(&rows, 0, &model));
    assert_string_equal(model.measurement_unit_code, "KGM");
    assert_string_equal(model.description, "Kilogram");
    assert_string_equal(model.abbreviation, "kg");

    measurement_unit_model_free(&model);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(builds_postgres_text_array_for_codes),
        cmocka_unit_test(hydrates_unit_row),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
