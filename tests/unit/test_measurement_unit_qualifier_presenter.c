#include "presenters/measurement_unit_qualifier_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

static void derives_identity(void **state)
{
    (void)state;

    MeasurementUnitQualifierModel model = {
        .measurement_unit_qualifier_code = "X",
        .description = "per hectolitre",
    };

    assert_string_equal(measurement_unit_qualifier_presenter_resource_type(), "measurement_unit_qualifier");
    assert_string_equal(measurement_unit_qualifier_presenter_id(&model), "X");
    assert_null(measurement_unit_qualifier_presenter_id(NULL));
}

static void formats_description(void **state)
{
    (void)state;

    MeasurementUnitQualifierModel model = {
        .measurement_unit_qualifier_code = "X",
        .description = "per hectolitre",
    };

    char *formatted = measurement_unit_qualifier_presenter_formatted_description(&model, "uk");
    assert_non_null(formatted);
    assert_string_equal(formatted, "Per hectolitre");
    free(formatted);

    assert_null(measurement_unit_qualifier_presenter_formatted_description(NULL, "uk"));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(derives_identity),
        cmocka_unit_test(formats_description),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
