#include "presenters/measure_component_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

static void derives_identity_and_resource_type(void **state)
{
    (void)state;

    MeasureComponentModel model = {
        .measure_sid = "123",
        .duty_expression_id = "01",
    };

    char *id = measure_component_presenter_id(&model);
    assert_non_null(id);
    assert_string_equal(id, "123-01");
    assert_string_equal(measure_component_presenter_resource_type(), "measure_component");

    free(id);
}

static void returns_null_for_incomplete_identity(void **state)
{
    (void)state;

    MeasureComponentModel missing_measure = {
        .duty_expression_id = "01",
    };
    MeasureComponentModel missing_duty_expression = {
        .measure_sid = "123",
    };

    assert_null(measure_component_presenter_id(NULL));
    assert_null(measure_component_presenter_id(&missing_measure));
    assert_null(measure_component_presenter_id(&missing_duty_expression));
}

static void derives_duty_expression_abbreviation(void **state)
{
    (void)state;

    MeasureComponentModel percent = {
        .duty_expression_id = "01",
    };
    MeasureComponentModel unknown = {
        .duty_expression_id = "XX",
    };

    assert_string_equal(measure_component_presenter_duty_expression_abbreviation(&percent), "%");
    assert_null(measure_component_presenter_duty_expression_abbreviation(NULL));
    assert_null(measure_component_presenter_duty_expression_abbreviation(&unknown));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(derives_identity_and_resource_type),
        cmocka_unit_test(returns_null_for_incomplete_identity),
        cmocka_unit_test(derives_duty_expression_abbreviation),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
