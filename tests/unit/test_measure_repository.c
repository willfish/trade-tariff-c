#include "repositories/measure_repository.h"

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

static void builds_postgres_array_param_from_numeric_sids(void **state)
{
    (void)state;

    const char *sids[] = { "107217", "bad", "", "107216" };
    char *param = measure_repository_sid_array_param(sids, sizeof(sids) / sizeof(sids[0]));

    assert_non_null(param);
    assert_string_equal(param, "{107217,107216}");

    free(param);
}

static void hydrates_measure_row(void **state)
{
    (void)state;

    char *values[21] = {
        copy_literal("123"),
        copy_literal("456"),
        copy_literal("8543400000"),
        copy_literal("103"),
        copy_literal("Third country duty"),
        copy_literal("1011"),
        copy_literal("1011"),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        copy_literal("false"),
        copy_literal("0"),
        copy_literal("C"),
        copy_literal("1"),
        copy_literal("R1234"),
        copy_literal("2024-01-01T00:00:00.000Z"),
        NULL,
        copy_literal("2024-01-01T00:00:00.000Z"),
        NULL,
    };
    unsigned char nulls[21] = {
        0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
        1, 1, 0, 0, 0, 0, 0, 0, 1, 0,
        1,
    };
    DbRows rows = {
        .values = values,
        .nulls = nulls,
        .rows = 1,
        .cols = 21,
        .error = NULL,
        .ok = 1,
    };

    MeasureModel model = { 0 };
    assert_true(measure_repository_hydrate_row(&rows, 0, &model));
    assert_string_equal(model.measure_sid, "123");
    assert_string_equal(model.goods_nomenclature_sid, "456");
    assert_string_equal(model.goods_nomenclature_item_id, "8543400000");
    assert_string_equal(model.measure_type_id, "103");
    assert_string_equal(model.measure_type_description, "Third country duty");
    assert_string_equal(model.geographical_area_id, "1011");
    assert_null(model.additional_code_sid);
    assert_null(model.ordernumber);
    assert_string_equal(model.trade_movement_code, "0");
    assert_string_equal(model.measure_type_series_id, "C");
    assert_string_equal(model.measure_generating_regulation_id, "R1234");
    assert_null(model.validity_end_date);
    assert_null(model.effective_end_date);

    measure_model_free(&model);
    for (size_t i = 0; i < 21; i++) {
        test_free(values[i]);
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(builds_postgres_array_param_from_numeric_sids),
        cmocka_unit_test(hydrates_measure_row),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
