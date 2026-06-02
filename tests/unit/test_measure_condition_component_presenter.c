#include "presenters/measure_condition_component_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

#include <cmocka.h>

static void derives_identity_and_resource_type(void **state)
{
    (void)state;

    MeasureConditionComponentModel model = {
        .measure_condition_sid = "789",
        .duty_expression_id = "17",
    };

    char *id = measure_condition_component_presenter_id(&model);

    assert_non_null(id);
    assert_string_equal(id, "789-17");
    assert_string_equal(measure_condition_component_presenter_resource_type(), "measure_condition_component");

    free(id);
}

static void returns_null_for_incomplete_identity(void **state)
{
    (void)state;

    MeasureConditionComponentModel missing_condition_sid = {
        .duty_expression_id = "17",
    };
    MeasureConditionComponentModel missing_duty_expression_id = {
        .measure_condition_sid = "789",
    };

    assert_null(measure_condition_component_presenter_id(NULL));
    assert_null(measure_condition_component_presenter_id(&missing_condition_sid));
    assert_null(measure_condition_component_presenter_id(&missing_duty_expression_id));
}

static void derives_duty_expression_abbreviation(void **state)
{
    (void)state;

    MeasureConditionComponentModel percentage = {
        .duty_expression_id = "01",
    };
    MeasureConditionComponentModel unknown = {
        .duty_expression_id = "XX",
    };

    assert_string_equal(measure_condition_component_presenter_duty_expression_abbreviation(&percentage), "%");
    assert_null(measure_condition_component_presenter_duty_expression_abbreviation(&unknown));
    assert_null(measure_condition_component_presenter_duty_expression_abbreviation(NULL));
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
