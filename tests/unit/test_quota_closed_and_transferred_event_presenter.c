#include "presenters/quota_closed_and_transferred_event_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

static void derives_identity_and_formatted_measurement_units(void **state)
{
    (void)state;

    QuotaClosedAndTransferredEventModel model = {
        .quota_definition_sid = "777",
        .occurrence_timestamp = "2024-01-31T00:00:00.000Z",
        .quota_definition_measurement_unit_description = "Kilogram",
        .quota_definition_measurement_unit_abbreviation = "kg",
        .target_quota_definition_measurement_unit_description = "Litre",
    };

    char *id = quota_closed_and_transferred_event_presenter_id(&model);
    char *source_unit = quota_closed_and_transferred_event_presenter_quota_definition_measurement_unit(&model);
    char *target_unit = quota_closed_and_transferred_event_presenter_target_quota_definition_measurement_unit(&model);

    assert_string_equal(quota_closed_and_transferred_event_presenter_resource_type(),
                        "quota_closed_and_transferred_event");
    assert_string_equal(id, "777-2024-01-31T00:00:00.000Z");
    assert_string_equal(source_unit, "Kilogram (kg)");
    assert_string_equal(target_unit, "Litre (Litre)");

    free(id);
    free(source_unit);
    free(target_unit);
}

static void returns_null_for_incomplete_identity_or_missing_unit_description(void **state)
{
    (void)state;

    QuotaClosedAndTransferredEventModel missing_sid = {
        .occurrence_timestamp = "2024-01-31T00:00:00.000Z",
    };
    QuotaClosedAndTransferredEventModel missing_description = {
        .quota_definition_sid = "777",
        .occurrence_timestamp = "2024-01-31T00:00:00.000Z",
    };

    assert_null(quota_closed_and_transferred_event_presenter_id(&missing_sid));
    assert_null(quota_closed_and_transferred_event_presenter_quota_definition_measurement_unit(&missing_description));
}

static void finds_event_for_target_definition(void **state)
{
    (void)state;

    QuotaClosedAndTransferredEventModelList events = { 0 };
    QuotaClosedAndTransferredEventModel first = {
        .quota_definition_sid = strdup("777"),
        .occurrence_timestamp = strdup("2024-01-31T00:00:00.000Z"),
        .target_quota_definition_sid = strdup("888"),
    };
    QuotaClosedAndTransferredEventModel second = {
        .quota_definition_sid = strdup("999"),
        .occurrence_timestamp = strdup("2024-02-01T00:00:00.000Z"),
        .target_quota_definition_sid = strdup("111"),
    };
    assert_true(quota_closed_and_transferred_event_model_list_append(&events, first));
    assert_true(quota_closed_and_transferred_event_model_list_append(&events, second));

    const QuotaClosedAndTransferredEventModel *found =
        quota_closed_and_transferred_event_presenter_for_target_definition(&events, "111");

    assert_non_null(found);
    assert_string_equal(found->quota_definition_sid, "999");
    assert_null(quota_closed_and_transferred_event_presenter_for_target_definition(&events, "000"));

    quota_closed_and_transferred_event_model_list_free(&events);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(derives_identity_and_formatted_measurement_units),
        cmocka_unit_test(returns_null_for_incomplete_identity_or_missing_unit_description),
        cmocka_unit_test(finds_event_for_target_definition),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
