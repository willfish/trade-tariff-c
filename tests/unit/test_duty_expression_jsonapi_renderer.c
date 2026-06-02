#include "renderers/duty_expression_jsonapi_renderer.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>
#include <yyjson.h>

static char *copy_literal(const char *value)
{
    size_t len = strlen(value);
    char *copy = malloc(len + 1);
    assert_non_null(copy);
    memcpy(copy, value, len + 1);
    return copy;
}

static MeasureComponentModel component(const char *measure_sid,
                                       const char *duty_expression_id,
                                       const char *amount)
{
    return (MeasureComponentModel){
        .measure_sid = copy_literal(measure_sid),
        .duty_expression_id = copy_literal(duty_expression_id),
        .duty_amount = copy_literal(amount),
        .duty_expression_description = copy_literal("Percentage duty"),
    };
}

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

static void renders_duty_expression_from_loaded_measure_components(void **state)
{
    (void)state;

    MeasureModel measure = {
        .measure_sid = copy_literal("123"),
    };
    MeasureComponentModelList components = { 0 };
    assert_true(measure_component_model_list_append(&components, component("123", "01", "8.5")));

    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *resource = duty_expression_jsonapi_resource(mut_doc, &measure, &components);
    assert_non_null(resource);
    yyjson_mut_doc_set_root(mut_doc, resource);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *attributes = yyjson_obj_get(root, "attributes");
    assert_string_equal(string_at(root, "id", NULL), "123-duty_expression");
    assert_string_equal(string_at(root, "type", NULL), "duty_expression");
    assert_string_equal(string_at(attributes, "base", NULL), "8.50 %");
    assert_string_equal(string_at(attributes, "formatted_base", NULL), "<span>8.50</span> %");
    assert_string_equal(string_at(attributes, "verbose_duty", NULL), "8.50%");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
    measure_component_model_list_free(&components);
    measure_model_free(&measure);
}

static void renders_empty_strings_when_measure_has_no_components(void **state)
{
    (void)state;

    MeasureModel measure = {
        .measure_sid = copy_literal("123"),
    };
    MeasureComponentModelList components = { 0 };
    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *resource = duty_expression_jsonapi_resource(mut_doc, &measure, &components);
    assert_non_null(resource);
    yyjson_mut_doc_set_root(mut_doc, resource);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *attributes = yyjson_obj_get(yyjson_doc_get_root(doc), "attributes");
    assert_string_equal(string_at(attributes, "base", NULL), "");
    assert_string_equal(string_at(attributes, "formatted_base", NULL), "");
    assert_string_equal(string_at(attributes, "verbose_duty", NULL), "");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
    measure_model_free(&measure);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(renders_duty_expression_from_loaded_measure_components),
        cmocka_unit_test(renders_empty_strings_when_measure_has_no_components),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
