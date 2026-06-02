#include "presenters/measure_type_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include <cmocka.h>

static void derives_identity(void **state)
{
    (void)state;

    MeasureTypeModel model = {
        .measure_type_id = "103",
        .description = "Third country duty",
    };

    assert_string_equal(measure_type_presenter_resource_type(), "measure_type");
    assert_string_equal(measure_type_presenter_id(&model), "103");
    assert_null(measure_type_presenter_id(NULL));
}

static void presents_nullable_numeric_codes(void **state)
{
    (void)state;

    MeasureTypePresentedCode numeric = measure_type_presenter_code("0");
    assert_false(numeric.is_null);
    assert_true(numeric.is_int);
    assert_int_equal(numeric.int_value, 0);

    MeasureTypePresentedCode blank = measure_type_presenter_code("");
    assert_true(blank.is_null);

    MeasureTypePresentedCode missing = measure_type_presenter_code(NULL);
    assert_true(missing.is_null);

    MeasureTypePresentedCode fallback = measure_type_presenter_code("A");
    assert_false(fallback.is_null);
    assert_false(fallback.is_int);
    assert_string_equal(fallback.string_value, "A");
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(derives_identity),
        cmocka_unit_test(presents_nullable_numeric_codes),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
