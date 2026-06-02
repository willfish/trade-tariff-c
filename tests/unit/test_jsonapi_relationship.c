#include "renderers/jsonapi_relationship.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>
#include <yyjson.h>

static void renders_single_resource_relationship(void **state)
{
    (void)state;

    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    assert_non_null(doc);

    yyjson_mut_val *relationship = jsonapi_single_relationship(doc, "measure", "123");
    assert_non_null(relationship);

    yyjson_mut_doc_set_root(doc, relationship);
    char *json = yyjson_mut_write(doc, 0, NULL);
    assert_non_null(json);
    assert_non_null(strstr(json, "\"id\":\"123\""));
    assert_non_null(strstr(json, "\"type\":\"measure\""));

    free(json);
    yyjson_mut_doc_free(doc);
}

static void renders_resource_object_shell(void **state)
{
    (void)state;

    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    assert_non_null(doc);

    yyjson_mut_val *resource = jsonapi_resource_object(doc, "commodity", "0101210000-80");
    assert_non_null(resource);

    yyjson_mut_doc_set_root(doc, resource);
    char *json = yyjson_mut_write(doc, 0, NULL);
    assert_non_null(json);
    assert_non_null(strstr(json, "\"id\":\"0101210000-80\""));
    assert_non_null(strstr(json, "\"type\":\"commodity\""));

    free(json);
    yyjson_mut_doc_free(doc);
}

static void renders_null_and_empty_array_relationships(void **state)
{
    (void)state;

    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    assert_non_null(doc);

    yyjson_mut_val *nullable = jsonapi_nullable_relationship(doc, "order_number", NULL);
    yyjson_mut_val *empty = jsonapi_empty_array_relationship(doc);
    assert_non_null(nullable);
    assert_non_null(empty);

    yyjson_mut_doc_free(doc);
}

static void adds_named_data_relationship(void **state)
{
    (void)state;

    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    assert_non_null(doc);

    yyjson_mut_val *relationships = yyjson_mut_obj(doc);
    yyjson_mut_val *data = yyjson_mut_arr(doc);
    assert_non_null(relationships);
    assert_non_null(data);
    assert_true(jsonapi_add_data_relationship(doc, relationships, "footnotes", data));

    yyjson_mut_doc_set_root(doc, relationships);
    char *json = yyjson_mut_write(doc, 0, NULL);
    assert_non_null(json);
    assert_non_null(strstr(json, "\"footnotes\":{\"data\":[]"));

    free(json);
    yyjson_mut_doc_free(doc);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(renders_single_resource_relationship),
        cmocka_unit_test(renders_resource_object_shell),
        cmocka_unit_test(renders_null_and_empty_array_relationships),
        cmocka_unit_test(adds_named_data_relationship),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
