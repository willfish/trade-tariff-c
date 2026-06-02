#include "formatters/description_formatter.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include <string.h>

static void preserves_colon_break_without_nbsp(void **state)
{
    (void)state;

    char *formatted = description_formatter_format("Axial Fan with:<br>-an air flow", 1);
    assert_non_null(formatted);
    assert_string_equal(formatted, "Axial Fan with:<br>-an air flow");
    free(formatted);
}

static void formats_markers_and_break_runs(void **state)
{
    (void)state;

    char *formatted = description_formatter_format("A<br><br><br><ul><br>  <li>x &apos; y</li><br></ul>", 1);
    assert_non_null(formatted);
    assert_string_equal(formatted, "A<br><ul><li>x &amp;apos; y</li></ul>");
    free(formatted);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(preserves_colon_break_without_nbsp),
        cmocka_unit_test(formats_markers_and_break_runs),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
