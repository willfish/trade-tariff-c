#include "renderers/measure_type_jsonapi_renderer.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>
#include <yyjson.h>

static yyjson_val *obj_path(yyjson_val *root, const char *first, const char *second)
{
    yyjson_val *value = yyjson_obj_get(root, first);
    return second == NULL || value == NULL ? value : yyjson_obj_get(value, second);
}

static const char *string_at(yyjson_val *root, const char *first, const char *second)
{
    yyjson_val *value = obj_path(root, first, second);
    return yyjson_is_str(value) ? yyjson_get_str(value) : NULL;
}

static void renders_measure_type_resource_from_typed_model(void **state)
{
    (void)state;

    MeasureTypeModel measure_type = {
        .measure_type_id = "103",
        .description = "Third country duty",
        .measure_type_series_id = "C",
        .measure_type_series_description = "Tariff preference measures",
        .measure_component_applicable_code = "1",
        .order_number_capture_code = "2",
        .trade_movement_code = "0",
        .validity_start_date = "1970-01-01 00:00:00",
    };
    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *resource = measure_type_jsonapi_resource(mut_doc, &measure_type);
    assert_non_null(resource);
    yyjson_mut_doc_set_root(mut_doc, resource);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *attributes = yyjson_obj_get(root, "attributes");
    assert_string_equal(string_at(root, "id", NULL), "103");
    assert_string_equal(string_at(root, "type", NULL), "measure_type");
    assert_string_equal(string_at(attributes, "id", NULL), "103");
    assert_string_equal(string_at(attributes, "description", NULL), "Third country duty");
    assert_string_equal(string_at(attributes, "measure_type_series_id", NULL), "C");
    assert_string_equal(string_at(attributes, "measure_type_series_description", NULL), "Tariff preference measures");
    assert_int_equal(yyjson_get_int(yyjson_obj_get(attributes, "measure_component_applicable_code")), 1);
    assert_int_equal(yyjson_get_int(yyjson_obj_get(attributes, "order_number_capture_code")), 2);
    assert_int_equal(yyjson_get_int(yyjson_obj_get(attributes, "trade_movement_code")), 0);
    assert_string_equal(string_at(attributes, "validity_start_date", NULL), "1970-01-01 00:00:00");
    assert_true(yyjson_is_null(yyjson_obj_get(attributes, "validity_end_date")));

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(renders_measure_type_resource_from_typed_model),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
