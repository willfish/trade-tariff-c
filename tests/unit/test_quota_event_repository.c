#include "repositories/quota_event_repository.h"

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

static void builds_postgres_text_array_param_from_definition_sids(void **state)
{
    (void)state;

    const char *sids[] = { "888", "bad", "", "999" };
    char *param = quota_event_repository_text_array_param(sids, sizeof(sids) / sizeof(sids[0]));

    assert_non_null(param);
    assert_string_equal(param, "{\"888\",\"999\"}");

    free(param);
}

static void hydrates_latest_quota_event_row(void **state)
{
    (void)state;

    char *values[3] = {
        copy_literal("888"),
        copy_literal("2024-02-01T00:00:00.000Z"),
        copy_literal("reopening"),
    };
    unsigned char nulls[3] = { 0 };
    DbRows rows = {
        .values = values,
        .nulls = nulls,
        .rows = 1,
        .cols = 3,
        .error = NULL,
        .ok = 1,
    };

    QuotaEventModel model = { 0 };
    assert_true(quota_event_repository_hydrate_row(&rows, 0, &model));
    assert_string_equal(model.quota_definition_sid, "888");
    assert_string_equal(model.occurrence_timestamp, "2024-02-01T00:00:00.000Z");
    assert_string_equal(model.event_type, "reopening");

    quota_event_model_free(&model);
    for (size_t i = 0; i < 3; i++) {
        test_free(values[i]);
    }
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(builds_postgres_text_array_param_from_definition_sids),
        cmocka_unit_test(hydrates_latest_quota_event_row),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
