#include "presenters/geographical_area_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include <cmocka.h>

static void derives_identity(void **state)
{
    (void)state;

    GeographicalAreaModel area = {
        .geographical_area_sid = "100",
        .geographical_area_id = "1011",
        .description = "All countries",
    };
    MeasureExcludedGeographicalAreaModel excluded = {
        .measure_sid = "123",
        .geographical_area_id = "AD",
    };

    assert_string_equal(geographical_area_presenter_resource_type(), "geographical_area");
    assert_string_equal(geographical_area_presenter_id(&area), "1011");
    assert_null(geographical_area_presenter_id(NULL));
    assert_string_equal(geographical_area_presenter_excluded_id(&excluded), "AD");
    assert_null(geographical_area_presenter_excluded_id(NULL));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(derives_identity),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
