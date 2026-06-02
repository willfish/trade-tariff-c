#include "renderers/measure_legal_act_jsonapi_renderer.h"

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

static void renders_legal_act_resource_from_typed_model(void **state)
{
    (void)state;

    MeasureLegalActModel model = {
        .measure_sid = "123",
        .measure_type_id = "103",
        .regulation_id = "R230001",
        .role = "1",
        .source = "generating",
        .validity_start_date = "2024-01-01T00:00:00.000Z",
        .officialjournal_number = "L1",
        .officialjournal_page = "10",
        .published_date = "2024-01-02",
        .information_text = "Regulation text",
    };

    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *resource = measure_legal_act_jsonapi_resource(mut_doc, &model, "uk");
    assert_non_null(resource);
    yyjson_mut_doc_set_root(mut_doc, resource);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *attributes = yyjson_obj_get(root, "attributes");
    assert_string_equal(string_at(root, "id"), "R230001");
    assert_string_equal(string_at(root, "type"), "legal_act");
    assert_string_equal(string_at(attributes, "regulation_code"), "R0001/23");
    assert_string_equal(string_at(attributes, "role"), "1");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
}

static void renders_relationship_refs_for_one_measure(void **state)
{
    (void)state;

    MeasureLegalActModelList legal_acts = { 0 };
    MeasureLegalActModel one = {
        .measure_sid = strdup("123"),
        .measure_type_id = strdup("103"),
        .regulation_id = strdup("R230001"),
        .source = strdup("generating"),
        .validity_start_date = strdup("2024-01-01T00:00:00.000Z"),
    };
    MeasureLegalActModel two = {
        .measure_sid = strdup("456"),
        .measure_type_id = strdup("103"),
        .regulation_id = strdup("R230002"),
        .source = strdup("generating"),
        .validity_start_date = strdup("2024-01-01T00:00:00.000Z"),
    };
    assert_true(measure_legal_act_model_list_append(&legal_acts, one));
    assert_true(measure_legal_act_model_list_append(&legal_acts, two));

    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *data = measure_legal_act_jsonapi_relationship_data_for_measure(mut_doc, &legal_acts, "123");
    assert_non_null(data);
    yyjson_mut_doc_set_root(mut_doc, data);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    assert_int_equal(yyjson_arr_size(root), 1);
    assert_string_equal(string_at(yyjson_arr_get(root, 0), "id"), "R230001");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
    measure_legal_act_model_list_free(&legal_acts);
}

static void renders_suspension_relationship_and_resource(void **state)
{
    (void)state;

    MeasureLegalActModelList legal_acts = { 0 };
    MeasureLegalActModel suspension = {
        .measure_sid = strdup("123"),
        .measure_type_id = strdup("103"),
        .regulation_id = strdup("R230002"),
        .source = strdup("suspension"),
        .effective_start_date = strdup("2024-01-01"),
        .effective_end_date = strdup("2024-12-31"),
    };
    MeasureLegalActModel generating = {
        .measure_sid = strdup("123"),
        .measure_type_id = strdup("103"),
        .regulation_id = strdup("R230001"),
        .source = strdup("generating"),
        .validity_start_date = strdup("2024-01-01T00:00:00.000Z"),
    };
    assert_true(measure_legal_act_model_list_append(&legal_acts, generating));
    assert_true(measure_legal_act_model_list_append(&legal_acts, suspension));

    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *root_obj = yyjson_mut_obj(mut_doc);
    assert_non_null(root_obj);
    assert_true(yyjson_mut_obj_add_val(mut_doc,
                                       root_obj,
                                       "relationship",
                                       measure_legal_act_jsonapi_suspension_relationship(mut_doc, &legal_acts, "123")));
    assert_true(yyjson_mut_obj_add_val(mut_doc,
                                       root_obj,
                                       "resource",
                                       measure_legal_act_jsonapi_suspension_resource(mut_doc, &legal_acts.items[1])));
    yyjson_mut_doc_set_root(mut_doc, root_obj);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *relationship_data = obj_path(yyjson_obj_get(root, "relationship"), "data", NULL);
    yyjson_val *resource = yyjson_obj_get(root, "resource");
    yyjson_val *attributes = yyjson_obj_get(resource, "attributes");
    assert_string_equal(string_at(relationship_data, "type"), "suspension_legal_act");
    assert_string_equal(string_at(relationship_data, "id"), "R230002");
    assert_string_equal(string_at(resource, "type"), "suspension_legal_act");
    assert_string_equal(string_at(attributes, "validity_start_date"), "2024-01-01");
    assert_string_equal(string_at(attributes, "validity_end_date"), "2024-12-31");
    assert_string_equal(string_at(attributes, "regulation_code"), "R0002/23");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
    measure_legal_act_model_list_free(&legal_acts);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(renders_legal_act_resource_from_typed_model),
        cmocka_unit_test(renders_relationship_refs_for_one_measure),
        cmocka_unit_test(renders_suspension_relationship_and_resource),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
