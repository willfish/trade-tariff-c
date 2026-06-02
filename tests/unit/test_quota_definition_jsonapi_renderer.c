#include "renderers/quota_definition_jsonapi_renderer.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>
#include <yyjson.h>

static const char *string_at(yyjson_val *root, const char *key)
{
    yyjson_val *value = yyjson_obj_get(root, key);
    return yyjson_is_str(value) ? yyjson_get_str(value) : NULL;
}

static yyjson_val *obj_path(yyjson_val *root, const char *first, const char *second)
{
    yyjson_val *value = yyjson_obj_get(root, first);
    return second == NULL || value == NULL ? value : yyjson_obj_get(value, second);
}

static void renders_definition_resource_from_typed_model(void **state)
{
    (void)state;

    QuotaDefinitionModel definition = {
        .quota_definition_sid = "888",
        .quota_order_number_id = "060004",
        .initial_volume = "1000.0",
        .validity_start_date = "2024-01-01T00:00:00.000Z",
        .balance = "900.0",
        .latest_critical_state = "Y",
        .measurement_unit_description = "Kilogram",
        .measurement_unit_abbreviation = "kg",
        .monetary_unit_code = "GBP",
        .measurement_unit_qualifier_code = "X",
        .last_allocation_date = "2024-02-01T00:00:00.000Z",
    };
    QuotaEventModel event = {
        .quota_definition_sid = "888",
        .occurrence_timestamp = "2024-02-01T00:00:00.000Z",
        .event_type = "balance",
    };
    QuotaEventModelList events = {
        .items = &event,
        .len = 1,
        .cap = 1,
    };

    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *resource = quota_definition_jsonapi_resource_with_events(mut_doc, &definition, &events, NULL);
    assert_non_null(resource);
    yyjson_mut_doc_set_root(mut_doc, resource);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *attributes = yyjson_obj_get(root, "attributes");
    assert_string_equal(string_at(root, "id"), "888");
    assert_string_equal(string_at(root, "type"), "definition");
    assert_string_equal(string_at(attributes, "status"), "Critical");
    assert_string_equal(string_at(attributes, "measurement_unit"), "Kilogram (kg)");
    assert_string_equal(string_at(attributes, "monetary_unit"), "GBP");
    assert_string_equal(string_at(attributes, "measurement_unit_qualifier"), "X");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
}

static void renders_incoming_transfer_relationship_when_managed_by_hmrc(void **state)
{
    (void)state;

    QuotaDefinitionModel definition = {
        .quota_definition_sid = "888",
        .quota_order_number_id = "060004",
        .initial_volume = "1000.0",
        .validity_start_date = "2024-01-01T00:00:00.000Z",
        .balance = "900.0",
    };
    QuotaClosedAndTransferredEventModel event = {
        .quota_definition_sid = "777",
        .occurrence_timestamp = "2024-01-31T00:00:00.000Z",
        .target_quota_definition_sid = "888",
    };
    QuotaClosedAndTransferredEventModelList events = {
        .items = &event,
        .len = 1,
        .cap = 1,
    };

    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *resource = quota_definition_jsonapi_resource_with_events(mut_doc, &definition, NULL, &events);
    assert_non_null(resource);
    yyjson_mut_doc_set_root(mut_doc, resource);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *relationships = yyjson_obj_get(root, "relationships");
    yyjson_val *incoming = obj_path(yyjson_obj_get(relationships, "incoming_quota_closed_and_transferred_event"), "data", NULL);
    assert_string_equal(string_at(incoming, "id"), "777-2024-01-31T00:00:00.000Z");
    assert_string_equal(string_at(incoming, "type"), "quota_closed_and_transferred_event");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(renders_definition_resource_from_typed_model),
        cmocka_unit_test(renders_incoming_transfer_relationship_when_managed_by_hmrc),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
