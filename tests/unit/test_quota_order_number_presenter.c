#include "presenters/quota_order_number_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include <cmocka.h>

static void derives_identity_type_and_number(void **state)
{
    (void)state;

    QuotaOrderNumberModel order_number = {
        .quota_order_number_id = "060004",
    };

    assert_string_equal(quota_order_number_presenter_resource_type(), "order_number");
    assert_string_equal(quota_order_number_presenter_id(&order_number), "060004");
    assert_string_equal(quota_order_number_presenter_number(&order_number), "060004");
    assert_null(quota_order_number_presenter_id(NULL));
}

static void finds_definition_by_order_number_id(void **state)
{
    (void)state;

    QuotaOrderNumberModel order_number = {
        .quota_order_number_id = "060004",
    };
    QuotaDefinitionModel definitions[] = {
        {
            .quota_definition_sid = "111",
            .quota_order_number_id = "060003",
        },
        {
            .quota_definition_sid = "222",
            .quota_order_number_id = "060004",
        },
    };
    QuotaDefinitionModelList list = {
        .items = definitions,
        .len = 2,
        .cap = 2,
    };

    assert_ptr_equal(quota_order_number_presenter_definition(&list, &order_number), &definitions[1]);
}

static void returns_null_without_matching_definition(void **state)
{
    (void)state;

    QuotaOrderNumberModel order_number = {
        .quota_order_number_id = "060004",
    };
    QuotaDefinitionModel definition = {
        .quota_definition_sid = "111",
        .quota_order_number_id = "060003",
    };
    QuotaDefinitionModelList list = {
        .items = &definition,
        .len = 1,
        .cap = 1,
    };

    assert_null(quota_order_number_presenter_definition(&list, &order_number));
    assert_null(quota_order_number_presenter_definition(NULL, &order_number));
    assert_null(quota_order_number_presenter_definition(&list, NULL));
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(derives_identity_type_and_number),
        cmocka_unit_test(finds_definition_by_order_number_id),
        cmocka_unit_test(returns_null_without_matching_definition),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
