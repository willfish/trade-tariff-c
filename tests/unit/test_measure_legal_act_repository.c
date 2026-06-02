#include "repositories/measure_legal_act_repository.h"

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
    char *param = measure_legal_act_repository_sid_array_param(sids, sizeof(sids) / sizeof(sids[0]));

    assert_non_null(param);
    assert_string_equal(param, "{123,-456}");

    free(param);
}

static void hydrates_measure_legal_act_row(void **state)
{
    (void)state;

    char *values[13] = {
        copy_literal("123"),
        copy_literal("103"),
        copy_literal("R230001"),
        copy_literal("1"),
        copy_literal("generating"),
        copy_literal("2024-01-01T00:00:00.000Z"),
        copy_literal("2024-12-31T00:00:00.000Z"),
        copy_literal("2024-01-01"),
        copy_literal("2024-12-31"),
        copy_literal("L1"),
        copy_literal("10"),
        copy_literal("2024-01-02"),
        copy_literal("Regulation text"),
    };
    unsigned char nulls[13] = { 0 };
    DbRows rows = {
        .values = values,
        .nulls = nulls,
        .rows = 1,
        .cols = 13,
        .error = NULL,
        .ok = 1,
    };

    MeasureLegalActModel model = { 0 };
    assert_true(measure_legal_act_repository_hydrate_row(&rows, 0, &model));
    assert_string_equal(model.measure_sid, "123");
    assert_string_equal(model.measure_type_id, "103");
    assert_string_equal(model.regulation_id, "R230001");
    assert_string_equal(model.source, "generating");
    assert_string_equal(model.effective_start_date, "2024-01-01");
    assert_string_equal(model.effective_end_date, "2024-12-31");
    assert_string_equal(model.officialjournal_page, "10");

    measure_legal_act_model_free(&model);
    for (size_t i = 0; i < 13; i++) {
        test_free(values[i]);
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(builds_postgres_array_param_from_numeric_measure_sids),
        cmocka_unit_test(hydrates_measure_legal_act_row),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
