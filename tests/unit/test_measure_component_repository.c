#include "repositories/measure_component_repository.h"
#include "sql_loader.h"

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
    char *param = measure_component_repository_sid_array_param(sids, sizeof(sids) / sizeof(sids[0]));

    assert_non_null(param);
    assert_string_equal(param, "{123,-456}");

    free(param);
}

static void hydrates_component_row(void **state)
{
    (void)state;

    char *values[7] = {
        copy_literal("123"),
        copy_literal("01"),
        copy_literal("8.95"),
        copy_literal("EUC"),
        copy_literal("KGM"),
        NULL,
        copy_literal("Percentage duty"),
    };
    unsigned char nulls[7] = { 0, 0, 0, 0, 0, 1, 0 };
    DbRows rows = {
        .values = values,
        .nulls = nulls,
        .rows = 1,
        .cols = 7,
        .error = NULL,
        .ok = 1,
    };

    MeasureComponentModel model = { 0 };
    assert_true(measure_component_repository_hydrate_row(&rows, 0, &model));
    assert_string_equal(model.measure_sid, "123");
    assert_string_equal(model.duty_expression_id, "01");
    assert_string_equal(model.duty_amount, "8.95");
    assert_string_equal(model.monetary_unit_code, "EUC");
    assert_string_equal(model.measurement_unit_code, "KGM");
    assert_null(model.measurement_unit_qualifier_code);
    assert_string_equal(model.duty_expression_description, "Percentage duty");

    measure_component_model_free(&model);
    for (size_t i = 0; i < 7; i++) {
        test_free(values[i]);
    }
}

static void resolved_meursing_sql_applies_rails_geographical_relevance(void **state)
{
    (void)state;

    char *sql = sql_loader_load("sql/measure_components/resolved_meursing_for_measure_sids.sql");

    assert_non_null(sql);
    assert_non_null(strstr(sql, "WHEN 'EU' THEN '1013'"));
    assert_non_null(strstr(sql, "expanded_memberships"));
    assert_non_null(strstr(sql, "membership.group_geographical_area_id = meursing_area.referenced_or_self_id"));
    assert_non_null(strstr(sql, "measure_excluded_geographical_areas exclusion"));
    assert_non_null(strstr(sql, "excluded_area.referenced_or_self_id = root_components.root_geographical_area_id"));
    assert_non_null(strstr(sql, "excluded_membership.group_geographical_area_id = excluded_area.referenced_or_self_id"));

    free(sql);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(builds_postgres_array_param_from_numeric_measure_sids),
        cmocka_unit_test(hydrates_component_row),
        cmocka_unit_test(resolved_meursing_sql_applies_rails_geographical_relevance),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
