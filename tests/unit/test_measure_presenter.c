#include "presenters/measure_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include <cmocka.h>

static void derives_measure_flags_from_typed_models(void **state)
{
    (void)state;

    MeasureModel measure = {
        .measure_sid = "-123",
        .measure_type_id = "305",
        .measure_type_series_id = "Q",
    };
    MeasureComponentModel components_items[] = {
        {
            .measure_sid = "-123",
            .duty_expression_id = "12",
        },
    };
    MeasureComponentModelList components = {
        .items = components_items,
        .len = 1,
        .cap = 1,
    };

    assert_string_equal(measure_presenter_resource_type(), "measure");
    assert_string_equal(measure_presenter_id(&measure), "-123");
    assert_int_equal(measure_presenter_numeric_id(&measure), -123);
    assert_string_equal(measure_presenter_origin(&measure), "uk");
    assert_true(measure_presenter_excise(&measure));
    assert_true(measure_presenter_vat(&measure));
    assert_true(measure_presenter_meursing(&measure, &components));
}

static void returns_false_for_non_matching_measure_flags(void **state)
{
    (void)state;

    MeasureModel measure = {
        .measure_sid = "456",
        .measure_type_id = "103",
        .measure_type_series_id = "C",
    };
    MeasureComponentModel components_items[] = {
        {
            .measure_sid = "999",
            .duty_expression_id = "12",
        },
        {
            .measure_sid = "456",
            .duty_expression_id = "01",
        },
    };
    MeasureComponentModelList components = {
        .items = components_items,
        .len = 2,
        .cap = 2,
    };

    assert_string_equal(measure_presenter_origin(&measure), "eu");
    assert_int_equal(measure_presenter_numeric_id(&measure), 456);
    assert_false(measure_presenter_excise(&measure));
    assert_false(measure_presenter_vat(&measure));
    assert_false(measure_presenter_meursing(&measure, &components));
}

static void returns_zero_numeric_id_for_missing_or_invalid_sid(void **state)
{
    (void)state;

    MeasureModel invalid = { .measure_sid = "abc" };
    MeasureModel missing = { 0 };

    assert_int_equal(measure_presenter_numeric_id(&invalid), 0);
    assert_int_equal(measure_presenter_numeric_id(&missing), 0);
    assert_int_equal(measure_presenter_numeric_id(NULL), 0);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(derives_measure_flags_from_typed_models),
        cmocka_unit_test(returns_false_for_non_matching_measure_flags),
        cmocka_unit_test(returns_zero_numeric_id_for_missing_or_invalid_sid),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
