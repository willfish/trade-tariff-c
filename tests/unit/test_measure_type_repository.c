#include "repositories/measure_type_repository.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

static void builds_postgres_text_array_for_ids(void **state)
{
    (void)state;

    const char *ids[] = { "103", NULL, "", "305" };
    char *param = measure_type_repository_id_array_param(ids, sizeof(ids) / sizeof(ids[0]));

    assert_non_null(param);
    assert_string_equal(param, "{\"103\",\"305\"}");

    free(param);
}

static void hydrates_measure_type_row(void **state)
{
    (void)state;

    char *values[] = {
        "103",
        "Third country duty",
        "C",
        "Tariff preference measures",
        "1",
        "2",
        "0",
        "1970-01-01 00:00:00",
        NULL,
    };
    unsigned char nulls[] = { 0, 0, 0, 0, 0, 0, 0, 0, 1 };
    DbRows rows = {
        .ok = 1,
        .rows = 1,
        .cols = 9,
        .values = values,
        .nulls = nulls,
    };
    MeasureTypeModel model = { 0 };

    assert_true(measure_type_repository_hydrate_row(&rows, 0, &model));
    assert_string_equal(model.measure_type_id, "103");
    assert_string_equal(model.description, "Third country duty");
    assert_string_equal(model.measure_type_series_id, "C");
    assert_string_equal(model.measure_type_series_description, "Tariff preference measures");
    assert_string_equal(model.measure_component_applicable_code, "1");
    assert_string_equal(model.order_number_capture_code, "2");
    assert_string_equal(model.trade_movement_code, "0");
    assert_string_equal(model.validity_start_date, "1970-01-01 00:00:00");
    assert_null(model.validity_end_date);

    measure_type_model_free(&model);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(builds_postgres_text_array_for_ids),
        cmocka_unit_test(hydrates_measure_type_row),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
