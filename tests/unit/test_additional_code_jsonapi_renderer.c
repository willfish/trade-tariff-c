#include "renderers/additional_code_jsonapi_renderer.h"

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

static void renders_additional_code_resource_from_typed_model(void **state)
{
    (void)state;

    AdditionalCodeModel model = {
        .additional_code_sid = "9001",
        .additional_code_type_id = "8",
        .additional_code = "123",
        .description = "Additional duty",
    };
    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);

    yyjson_mut_val *resource = additional_code_jsonapi_resource(mut_doc, &model);
    assert_non_null(resource);
    yyjson_mut_doc_set_root(mut_doc, resource);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *attributes = yyjson_obj_get(root, "attributes");
    assert_string_equal(string_at(root, "id", NULL), "9001");
    assert_string_equal(string_at(root, "type", NULL), "additional_code");
    assert_string_equal(string_at(attributes, "code", NULL), "8123");
    assert_string_equal(string_at(attributes, "description", NULL), "Additional duty");
    assert_string_equal(string_at(attributes, "formatted_description", NULL), "Additional duty");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(renders_additional_code_resource_from_typed_model),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
