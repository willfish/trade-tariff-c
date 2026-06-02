#include "renderers/quota_order_number_jsonapi_renderer.h"

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

static void renders_order_number_resource_with_definition_relationship(void **state)
{
    (void)state;

    QuotaOrderNumberModel order_number = {
        .quota_order_number_sid = "777",
        .quota_order_number_id = "060004",
    };
    QuotaDefinitionModel definition = {
        .quota_definition_sid = "888",
        .quota_order_number_id = "060004",
    };
    QuotaDefinitionModelList definitions = {
        .items = &definition,
        .len = 1,
        .cap = 1,
    };

    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *resource = quota_order_number_jsonapi_resource(mut_doc, &order_number, &definitions);
    assert_non_null(resource);
    yyjson_mut_doc_set_root(mut_doc, resource);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *relationships = yyjson_obj_get(root, "relationships");
    yyjson_val *definition_ref = obj_path(yyjson_obj_get(relationships, "definition"), "data", NULL);
    assert_string_equal(string_at(root, "id", NULL), "060004");
    assert_string_equal(string_at(root, "type", NULL), "order_number");
    assert_string_equal(string_at(yyjson_obj_get(root, "attributes"), "number", NULL), "060004");
    assert_string_equal(string_at(definition_ref, "id", NULL), "888");
    assert_string_equal(string_at(definition_ref, "type", NULL), "definition");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(renders_order_number_resource_with_definition_relationship),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
