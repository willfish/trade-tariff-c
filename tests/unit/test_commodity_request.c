#include "models/commodity.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

static void validates_actual_date_from_request(void **state)
{
    (void)state;

    CommodityRequest request = commodity_request_new("uk", "0101210000");
    request.actual_date = "2026-05-22";
    assert_int_equal(commodity_request_validate(&request), COMMODITY_REQUEST_VALID);

    request.actual_date = "2026-5-22";
    assert_int_equal(commodity_request_validate(&request), COMMODITY_REQUEST_BAD_ACTUAL_DATE);
}

static void defaults_to_no_actual_date(void **state)
{
    (void)state;

    CommodityRequest request = commodity_request_new("xi", "0101210000");
    assert_null(request.actual_date);
    assert_int_equal(commodity_request_validate(&request), COMMODITY_REQUEST_VALID);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(validates_actual_date_from_request),
        cmocka_unit_test(defaults_to_no_actual_date),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
