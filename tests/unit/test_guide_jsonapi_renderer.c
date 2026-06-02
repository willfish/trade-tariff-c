#include "renderers/guide_jsonapi_renderer.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>
#include <yyjson.h>

static const char *string_at(yyjson_val *root, const char *first, const char *second)
{
    yyjson_val *value = yyjson_obj_get(root, first);
    if (second != NULL && value != NULL) {
        value = yyjson_obj_get(value, second);
    }
    return yyjson_is_str(value) ? yyjson_get_str(value) : NULL;
}

static void renders_guide_resource(void **state)
{
    (void)state;

    GuideModel guide = {
        .id = "12",
        .title = "Using a commodity code",
        .url = "https://www.gov.uk/guidance/using-a-commodity-code",
    };

    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *resource = guide_jsonapi_resource(mut_doc, &guide);
    assert_non_null(resource);
    yyjson_mut_doc_set_root(mut_doc, resource);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    assert_string_equal(string_at(root, "id", NULL), "12");
    assert_string_equal(string_at(root, "type", NULL), "guide");
    assert_string_equal(string_at(root, "attributes", "title"), "Using a commodity code");
    assert_string_equal(string_at(root, "attributes", "url"), "https://www.gov.uk/guidance/using-a-commodity-code");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(renders_guide_resource),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
