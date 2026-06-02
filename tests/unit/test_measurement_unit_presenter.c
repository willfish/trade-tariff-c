#include "presenters/measurement_unit_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

static void derives_identity(void **state)
{
    (void)state;

    MeasurementUnitModel model = {
        .measurement_unit_code = "KGM",
        .description = "Kilogram",
    };

    assert_string_equal(measurement_unit_presenter_resource_type(), "measurement_unit");
    assert_string_equal(measurement_unit_presenter_id(&model), "KGM");
    assert_null(measurement_unit_presenter_id(NULL));
}

static void formats_verbose_expansion(void **state)
{
    (void)state;

    char *prefix = measurement_unit_presenter_verbose_expansion_prefix("pairs (pa)");
    assert_non_null(prefix);
    assert_string_equal(prefix, "number of pairs (pa)");
    assert_null(measurement_unit_presenter_verbose_expansion_prefix(NULL));
    assert_null(measurement_unit_presenter_verbose_expansion_prefix(""));
    free(prefix);
}

static void normalizes_verbose_text(void **state)
{
    (void)state;

    char *normalized = measurement_unit_presenter_normalize_verbose_owned(strdup("90\xC2\xA0% dry"));
    assert_non_null(normalized);
    assert_string_equal(normalized, "90% dry");
    free(normalized);

    char *carats = measurement_unit_presenter_normalize_verbose_owned(
        strdup("carats (one metric carat = 2 x 10<sup>-4</sup>kg) (c/k)"));
    assert_non_null(carats);
    assert_string_equal(carats, "carat (metric) (c/k)");
    free(carats);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(derives_identity),
        cmocka_unit_test(formats_verbose_expansion),
        cmocka_unit_test(normalizes_verbose_text),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
