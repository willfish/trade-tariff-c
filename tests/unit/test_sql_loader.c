#include "sql_loader.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include <string.h>

static void applies_template_bindings(void **state)
{
    (void)state;

    const SqlTemplateBinding bindings[] = {
        { "SCHEMA", "uk" },
        { "ACTUAL_DATE", "'2026-05-31'::date" },
    };

    char *sql = sql_loader_apply_bindings(
        "SELECT * FROM {{SCHEMA}}.measures WHERE validity_start_date <= {{ACTUAL_DATE}}",
        bindings,
        2);

    assert_non_null(sql);
    assert_string_equal(sql, "SELECT * FROM uk.measures WHERE validity_start_date <= '2026-05-31'::date");
    free(sql);
}

static void leaves_unbound_tokens_visible(void **state)
{
    (void)state;

    const SqlTemplateBinding bindings[] = {
        { "SCHEMA", "xi" },
    };

    char *sql = sql_loader_apply_bindings("{{SCHEMA}} {{MISSING}}", bindings, 1);
    assert_non_null(sql);
    assert_string_equal(sql, "xi {{MISSING}}");
    free(sql);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(applies_template_bindings),
        cmocka_unit_test(leaves_unbound_tokens_visible),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
