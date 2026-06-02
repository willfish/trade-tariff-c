#include "renderers/measure_condition_component_jsonapi_renderer.h"

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

static MeasureConditionComponentModel component(const char *condition_sid,
                                                const char *duty_expression_id)
{
    return (MeasureConditionComponentModel){
        .measure_condition_sid = copy_literal(condition_sid),
        .duty_expression_id = copy_literal(duty_expression_id),
        .duty_amount = copy_literal("8.95"),
        .monetary_unit_code = copy_literal("EUC"),
        .measurement_unit_code = copy_literal("KGM"),
        .measurement_unit_qualifier_code = copy_literal("X"),
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

static void renders_condition_component_resource_from_typed_model(void **state)
{
    (void)state;

    MeasureConditionComponentModel model = component("789", "01");
    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *resource = measure_condition_component_jsonapi_resource(mut_doc, &model);
    assert_non_null(resource);
    yyjson_mut_doc_set_root(mut_doc, resource);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *attributes = yyjson_obj_get(root, "attributes");

    assert_string_equal(string_at(root, "id", NULL), "789-01");
    assert_string_equal(string_at(root, "type", NULL), "measure_condition_component");
    assert_string_equal(string_at(attributes, "duty_expression_id", NULL), "01");
    assert_true(yyjson_is_num(yyjson_obj_get(attributes, "duty_amount")));
    assert_string_equal(string_at(attributes, "monetary_unit_abbreviation", NULL), "EUR (EUC)");
    assert_string_equal(string_at(attributes, "duty_expression_abbreviation", NULL), "%");
    assert_string_equal(string_at(attributes, "measure_condition_sid", NULL), "789");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
    measure_condition_component_model_free(&model);
}

static void renders_relationship_refs_for_one_condition(void **state)
{
    (void)state;

    MeasureConditionComponentModelList components = { 0 };
    assert_true(measure_condition_component_model_list_append(&components, component("789", "01")));
    assert_true(measure_condition_component_model_list_append(&components, component("790", "01")));
    assert_true(measure_condition_component_model_list_append(&components, component("789", "99")));

    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *data = measure_condition_component_jsonapi_relationship_data_for_condition(mut_doc, &components, "789");
    assert_non_null(data);
    yyjson_mut_doc_set_root(mut_doc, data);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    assert_int_equal(yyjson_arr_size(root), 2);
    assert_string_equal(string_at(yyjson_arr_get(root, 0), "id", NULL), "789-01");
    assert_string_equal(string_at(yyjson_arr_get(root, 1), "id", NULL), "789-99");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
    measure_condition_component_model_list_free(&components);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(renders_condition_component_resource_from_typed_model),
        cmocka_unit_test(renders_relationship_refs_for_one_condition),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
