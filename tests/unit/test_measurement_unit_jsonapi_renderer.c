#include "renderers/measurement_unit_jsonapi_renderer.h"

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

static void renders_measurement_unit_resource(void **state)
{
    (void)state;

    MeasurementUnitModel unit = {
        .measurement_unit_code = "KGM",
        .description = "Kilogram",
        .abbreviation = "kg",
    };
    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *resource = measurement_unit_jsonapi_resource(mut_doc, &unit);
    assert_non_null(resource);
    yyjson_mut_doc_set_root(mut_doc, resource);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *attributes = yyjson_obj_get(root, "attributes");
    assert_string_equal(string_at(root, "id"), "KGM");
    assert_string_equal(string_at(root, "type"), "measurement_unit");
    assert_string_equal(string_at(attributes, "description"), "Kilogram");
    assert_string_equal(string_at(attributes, "measurement_unit_code"), "KGM");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(renders_measurement_unit_resource),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
