#include "json_builder.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>
#include <string.h>

static void writes_object_values(void **state)
{
    (void)state;

    JsonBuilder *builder = json_builder_new_object();
    assert_non_null(builder);
    assert_true(json_builder_add_string(builder, "type", "commodity"));
    assert_true(json_builder_add_int(builder, "sid", 107095));
    assert_true(json_builder_add_bool(builder, "leaf", 1));
    assert_true(json_builder_add_null(builder, "validity_end_date"));

    size_t len = 0;
    char *json = json_builder_write(builder, &len);
    assert_non_null(json);
    assert_true(len > 0);
    assert_non_null(strstr(json, "\"type\":\"commodity\""));
    assert_non_null(strstr(json, "\"sid\":107095"));
    assert_non_null(strstr(json, "\"leaf\":true"));
    assert_non_null(strstr(json, "\"validity_end_date\":null"));

    free(json);
    json_builder_free(builder);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(writes_object_values),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
