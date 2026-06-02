#include "renderers/measure_condition_jsonapi_renderer.h"

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

static MeasureConditionModel condition(const char *measure_sid,
                                       const char *condition_sid)
{
    return (MeasureConditionModel){
        .measure_condition_sid = copy_literal(condition_sid),
        .measure_sid = copy_literal(measure_sid),
        .condition_code = copy_literal("B"),
        .component_sequence_number = copy_literal("1"),
        .condition_duty_amount = copy_literal("12.5"),
        .condition_monetary_unit_code = copy_literal("EUC"),
        .condition_measurement_unit_code = copy_literal("KGM"),
        .certificate_type_code = copy_literal("C"),
        .certificate_code = copy_literal("990"),
        .action_code = copy_literal("01"),
        .action = copy_literal("Produce certificate"),
        .measure_condition_code_description = copy_literal("Condition text"),
        .certificate_type_description = copy_literal("Certificate type"),
        .certificate_description = copy_literal("Certificate description"),
        .measurement_unit_description = copy_literal("Kilogram"),
    };
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
    };
}

static Appendix5aModel appendix_5a(const char *certificate_type_code,
                                   const char *certificate_code)
{
    return (Appendix5aModel){
        .certificate_type_code = copy_literal(certificate_type_code),
        .certificate_code = copy_literal(certificate_code),
        .cds_guidance = copy_literal("CDS guidance text"),
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

static void renders_condition_resource_from_typed_model(void **state)
{
    (void)state;

    MeasureConditionModel model = condition("123", "789");
    MeasureConditionComponentModelList components = { 0 };
    Appendix5aModelList appendix_5as = { 0 };
    assert_true(measure_condition_component_model_list_append(&components, component("789", "01")));
    assert_true(appendix_5a_model_list_append(&appendix_5as, appendix_5a("C", "990")));
    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *resource = measure_condition_jsonapi_resource(mut_doc, &model, &components, &appendix_5as);
    assert_non_null(resource);
    yyjson_mut_doc_set_root(mut_doc, resource);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    yyjson_val *attributes = yyjson_obj_get(root, "attributes");
    yyjson_val *relationships = yyjson_obj_get(root, "relationships");
    yyjson_val *condition_code_data = obj_path(yyjson_obj_get(relationships, "measure_condition_code"), "data", NULL);
    yyjson_val *components_data = obj_path(yyjson_obj_get(relationships, "measure_condition_components"), "data", NULL);

    assert_string_equal(string_at(root, "id", NULL), "789");
    assert_string_equal(string_at(root, "type", NULL), "measure_condition");
    assert_string_equal(string_at(attributes, "action", NULL), "Produce certificate");
    assert_string_equal(string_at(attributes, "condition", NULL), "B: Condition text");
    assert_string_equal(string_at(attributes, "document_code", NULL), "C990");
    assert_string_equal(string_at(attributes, "duty_expression", NULL), "<span>8.95</span> % EUR (EUC) / KGM");
    assert_string_equal(string_at(attributes, "guidance_cds", NULL), "CDS guidance text");
    assert_string_equal(string_at(attributes, "measure_condition_class", NULL), "threshold");
    assert_string_equal(string_at(attributes, "monetary_unit_abbreviation", NULL), "EUR (EUC)");
    assert_string_equal(string_at(attributes, "requirement", NULL), "Certificate type: Certificate description");
    assert_true(yyjson_is_null(yyjson_obj_get(attributes, "requirement_operator")));
    assert_string_equal(string_at(attributes, "threshold_unit_type", NULL), "price");
    assert_true(yyjson_is_num(yyjson_obj_get(attributes, "condition_duty_amount")));
    assert_string_equal(string_at(condition_code_data, "id", NULL), "B");
    assert_string_equal(string_at(condition_code_data, "type", NULL), "measure_condition_code");
    assert_int_equal(yyjson_arr_size(components_data), 1);
    assert_string_equal(string_at(yyjson_arr_get(components_data, 0), "id", NULL), "789-01");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
    measure_condition_model_free(&model);
    measure_condition_component_model_list_free(&components);
    appendix_5a_model_list_free(&appendix_5as);
}

static void renders_relationship_refs_for_one_measure(void **state)
{
    (void)state;

    MeasureConditionModelList conditions = { 0 };
    assert_true(measure_condition_model_list_append(&conditions, condition("123", "789")));
    assert_true(measure_condition_model_list_append(&conditions, condition("456", "790")));
    assert_true(measure_condition_model_list_append(&conditions, condition("123", "791")));

    yyjson_mut_doc *mut_doc = yyjson_mut_doc_new(NULL);
    assert_non_null(mut_doc);
    yyjson_mut_val *data = measure_condition_jsonapi_relationship_data_for_measure(mut_doc, &conditions, "123");
    assert_non_null(data);
    yyjson_mut_doc_set_root(mut_doc, data);

    size_t len = 0;
    char *json = yyjson_mut_write(mut_doc, YYJSON_WRITE_NOFLAG, &len);
    assert_non_null(json);
    yyjson_doc *doc = yyjson_read(json, len, 0);
    assert_non_null(doc);

    yyjson_val *root = yyjson_doc_get_root(doc);
    assert_int_equal(yyjson_arr_size(root), 2);
    assert_string_equal(string_at(yyjson_arr_get(root, 0), "id", NULL), "789");
    assert_string_equal(string_at(yyjson_arr_get(root, 1), "id", NULL), "791");

    yyjson_doc_free(doc);
    free(json);
    yyjson_mut_doc_free(mut_doc);
    measure_condition_model_list_free(&conditions);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(renders_condition_resource_from_typed_model),
        cmocka_unit_test(renders_relationship_refs_for_one_measure),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
