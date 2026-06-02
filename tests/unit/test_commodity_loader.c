#include "loaders/commodity_load_plan.h"
#include "loaders/commodity_loader.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include <cmocka.h>

static void rejects_missing_context_values(void **state)
{
    (void)state;

    CommodityAggregate aggregate = { 0 };
    CommodityRequestContext missing_schema = {
        .schema = NULL,
        .actual_date = NULL,
        .commodity_id = "8543400000",
    };
    CommodityRequestContext missing_commodity = {
        .schema = "uk",
        .actual_date = NULL,
        .commodity_id = NULL,
    };

    assert_false(commodity_loader_load(NULL, &aggregate));
    assert_false(commodity_loader_load(&missing_schema, &aggregate));
    assert_false(commodity_loader_load(&missing_commodity, &aggregate));
    assert_false(commodity_loader_load(&(CommodityRequestContext){
        .schema = "uk",
        .actual_date = NULL,
        .commodity_id = "8543400000",
    }, NULL));
    assert_false(commodity_loader_load_with_plan(&(CommodityRequestContext){
        .schema = "uk",
        .actual_date = NULL,
        .commodity_id = "8543400000",
    }, NULL, &aggregate));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(rejects_missing_context_values),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
