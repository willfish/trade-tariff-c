#include "domain/jsonapi_include_index.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>

#include <cmocka.h>
#include <yyjson.h>

static yyjson_mut_val *resource(yyjson_mut_doc *doc, const char *type, const char *id)
{
    yyjson_mut_val *value = yyjson_mut_obj(doc);
    assert_non_null(value);
    assert_true(yyjson_mut_obj_add_strcpy(doc, value, "type", type));
    assert_true(yyjson_mut_obj_add_strcpy(doc, value, "id", id));
    return value;
}

static void appends_each_type_id_once(void **state)
{
    (void)state;

    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    assert_non_null(doc);
    yyjson_mut_val *included = yyjson_mut_arr(doc);
    assert_non_null(included);
    JsonApiIncludeIndex index = { 0 };

    assert_true(jsonapi_include_index_append_resource(&index, included, "footnote", "CD123", resource(doc, "footnote", "CD123")));
    assert_true(jsonapi_include_index_append_resource(&index, included, "footnote", "CD123", resource(doc, "footnote", "CD123")));
    assert_true(jsonapi_include_index_append_resource(&index, included, "measure", "CD123", resource(doc, "measure", "CD123")));

    assert_true(jsonapi_include_index_seen(&index, "footnote", "CD123"));
    assert_true(jsonapi_include_index_seen(&index, "measure", "CD123"));
    assert_int_equal(yyjson_mut_arr_size(included), 2);

    jsonapi_include_index_free(&index);
    yyjson_mut_doc_free(doc);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(appends_each_type_id_once),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
