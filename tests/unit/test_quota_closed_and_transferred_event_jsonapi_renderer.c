#include "renderers/quota_closed_and_transferred_event_jsonapi_renderer.h"

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

static void renders_event_resource_from_typed_model(void **state)
{
    (void)state;

    QuotaClosedAndTransferredEventModel event = {
        .quota_definition_sid = "777",
        .occurrence_timestamp = "2024-01-31T00:00:00.000Z",
        .target_quota_definition_sid = "888",
        .closing_date = "2024-01-31",
        .transferred_amount = "12.5",
        .quota_definition_validity_start_date = "2023-01-01T00:00:00.000Z",
        .quota_definition_validity_end_date = "2023-12-31T00:00:00.000Z",
        .quota_definition_measurement_unit_description = "Kilogram",
        .quota_definition_measurement_unit_abbreviation = "kg",
        .target_quota_definition_validity_start_date = "2024-01-01T00:00:00.000Z",
        .target_quota_definition_validity_end_date = "2024-12-31T00:00:00.000Z",
        .target_quota_definition_measurement_unit_description = "Kilogram",
        .target_quota_definition_measurement_unit_abbreviation = "kg",
    };

    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *resource = quota_closed_and_transferred_event_jsonapi_resource(mut_doc, &event);
    assert_non_null(resource);
    yyjson_mut_doc_set_root(mut_doc, resource);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *attributes = yyjson_obj_get(root, "attributes");
    assert_string_equal(string_at(root, "id"), "777-2024-01-31T00:00:00.000Z");
    assert_string_equal(string_at(root, "type"), "quota_closed_and_transferred_event");
    assert_string_equal(string_at(attributes, "closing_date"), "2024-01-31");
    assert_float_equal(yyjson_get_num(yyjson_obj_get(attributes, "transferred_amount")), 12.5, 0.001);
    assert_string_equal(string_at(attributes, "quota_definition_measurement_unit"), "Kilogram (kg)");
    assert_string_equal(string_at(attributes, "target_quota_definition_measurement_unit"), "Kilogram (kg)");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(renders_event_resource_from_typed_model),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
