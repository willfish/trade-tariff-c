#include "presenters/duty_expression_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include <cmocka.h>

static void derives_identity_and_attributes(void **state)
{
    (void)state;

    DutyExpressionModel model = {
        .id = "123-duty_expression",
        .base = "8.50 %",
        .formatted_base = "<span>8.50</span> %",
        .verbose_duty = "8.50%",
    };

    assert_string_equal(duty_expression_presenter_resource_type(), "duty_expression");
    assert_string_equal(duty_expression_presenter_id(&model), "123-duty_expression");
    assert_string_equal(duty_expression_presenter_base(&model), "8.50 %");
    assert_string_equal(duty_expression_presenter_formatted_base(&model), "<span>8.50</span> %");
    assert_string_equal(duty_expression_presenter_verbose_duty(&model), "8.50%");
}

static void handles_absent_model(void **state)
{
    (void)state;

    assert_null(duty_expression_presenter_id(NULL));
    assert_null(duty_expression_presenter_base(NULL));
    assert_null(duty_expression_presenter_formatted_base(NULL));
    assert_null(duty_expression_presenter_verbose_duty(NULL));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(derives_identity_and_attributes),
        cmocka_unit_test(handles_absent_model),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
