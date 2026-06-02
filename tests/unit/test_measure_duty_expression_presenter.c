#include "presenters/measure_duty_expression_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

#include <cmocka.h>

static void derives_identity_and_builds_expression_model(void **state)
{
    (void)state;

    MeasureModel measure = {
        .measure_sid = "123",
    };
    MeasureComponentModel components_items[] = {
        {
            .measure_sid = "123",
            .duty_expression_id = "01",
            .duty_amount = "8.500000",
        },
    };
    MeasureComponentModelList components = {
        .items = components_items,
        .len = 1,
        .cap = 1,
    };

    char *id = measure_duty_expression_presenter_id(&measure);
    DutyExpressionModel expression = measure_duty_expression_presenter_build(&measure, &components);

    assert_string_equal(id, "123-duty_expression");
    assert_string_equal(expression.id, "123-duty_expression");
    assert_string_equal(expression.base, "8.50 %");
    assert_string_equal(expression.formatted_base, "<span>8.50</span> %");
    assert_string_equal(expression.verbose_duty, "8.50%");

    free(id);
    duty_expression_model_free(&expression);
}

static void returns_null_identity_without_measure_sid(void **state)
{
    (void)state;

    assert_null(measure_duty_expression_presenter_id(NULL));
    assert_null(measure_duty_expression_presenter_id(&(MeasureModel){ 0 }));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(derives_identity_and_builds_expression_model),
        cmocka_unit_test(returns_null_identity_without_measure_sid),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
