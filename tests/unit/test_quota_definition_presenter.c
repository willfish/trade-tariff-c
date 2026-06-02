#include "presenters/quota_definition_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

static void derives_identity_and_resource_types(void **state)
{
    (void)state;

    QuotaDefinitionModel definition = {
        .quota_definition_sid = "888",
    };

    assert_string_equal(quota_definition_presenter_resource_type(), "quota_definition");
    assert_string_equal(quota_definition_presenter_jsonapi_resource_type(), "definition");
    assert_string_equal(quota_definition_presenter_id(&definition), "888");
    assert_null(quota_definition_presenter_id(NULL));
}

static void status_uses_latest_event_not_any_exhaustion(void **state)
{
    (void)state;

    QuotaDefinitionModel definition = {
        .quota_definition_sid = "888",
        .critical_state = "N",
    };
    QuotaEventModel latest_event = {
        .quota_definition_sid = "888",
        .occurrence_timestamp = "2024-02-01T00:00:00.000Z",
        .event_type = "reopening",
    };
    QuotaEventModelList events = {
        .items = &latest_event,
        .len = 1,
        .cap = 1,
    };

    assert_string_equal(quota_definition_presenter_status(&definition, &events), "Open");
}

static void status_marks_open_latest_event_critical_when_latest_critical_is_active(void **state)
{
    (void)state;

    QuotaDefinitionModel definition = {
        .quota_definition_sid = "888",
        .critical_state = "N",
        .latest_critical_state = "Y",
    };
    QuotaEventModel latest_event = {
        .quota_definition_sid = "888",
        .occurrence_timestamp = "2024-02-01T00:00:00.000Z",
        .event_type = "balance",
    };
    QuotaEventModelList events = {
        .items = &latest_event,
        .len = 1,
        .cap = 1,
    };

    assert_string_equal(quota_definition_presenter_status(&definition, &events), "Critical");
}

static void active_periods_override_latest_events(void **state)
{
    (void)state;

    QuotaDefinitionModel suspended = {
        .quota_definition_sid = "888",
        .suspension_period_start_date = "2024-01-01",
        .suspension_period_end_date = "2024-01-31",
    };
    QuotaDefinitionModel blocked = {
        .quota_definition_sid = "999",
        .blocking_period_start_date = "2024-01-01",
        .blocking_period_end_date = "2024-01-31",
    };

    assert_string_equal(quota_definition_presenter_status(&suspended, NULL), "Suspended");
    assert_string_equal(quota_definition_presenter_status(&blocked, NULL), "Blocked");
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(derives_identity_and_resource_types),
        cmocka_unit_test(status_uses_latest_event_not_any_exhaustion),
        cmocka_unit_test(status_marks_open_latest_event_critical_when_latest_critical_is_active),
        cmocka_unit_test(active_periods_override_latest_events),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
