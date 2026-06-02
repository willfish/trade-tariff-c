#include "presenters/measure_condition_code_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include <cmocka.h>

static void derives_identity(void **state)
{
    (void)state;

    MeasureConditionCodeModel model = {
        .condition_code = "B",
        .description = "Presentation of a certificate/licence/document",
    };

    assert_string_equal(measure_condition_code_presenter_resource_type(), "measure_condition_code");
    assert_string_equal(measure_condition_code_presenter_id(&model), "B");
    assert_null(measure_condition_code_presenter_id(NULL));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(derives_identity),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
