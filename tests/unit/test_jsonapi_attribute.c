#include "renderers/jsonapi_attribute.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>
#include <yyjson.h>

static void adds_string_null_and_numeric_string_values(void **state)
{
    (void)state;

    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    assert_non_null(doc);

    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    assert_non_null(attributes);
    assert_true(jsonapi_add_string_or_null(doc, attributes, "description", "VAT"));
    assert_true(jsonapi_add_string_or_null(doc, attributes, "missing", NULL));
    assert_true(jsonapi_add_number_string_or_null(doc, attributes, "duty_amount", "8.95"));
    assert_true(jsonapi_add_number_string_or_null(doc, attributes, "opaque_amount", "8.95%"));

    yyjson_mut_doc_set_root(doc, attributes);
    char *json = yyjson_mut_write(doc, 0, NULL);
    assert_non_null(json);
    assert_non_null(strstr(json, "\"description\":\"VAT\""));
    assert_non_null(strstr(json, "\"missing\":null"));
    assert_non_null(strstr(json, "\"duty_amount\":8.95"));
    assert_non_null(strstr(json, "\"opaque_amount\":\"8.95%\""));

    free(json);
    yyjson_mut_doc_free(doc);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(adds_string_null_and_numeric_string_values),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
