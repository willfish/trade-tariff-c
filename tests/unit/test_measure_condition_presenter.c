#include "presenters/measure_condition_presenter.h"

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <cmocka.h>

static char *copy_literal(const char *value)
{
    size_t len = strlen(value);
    char *copy = malloc(len + 1);
    assert_non_null(copy);
    memcpy(copy, value, len + 1);
    return copy;
}

static MeasureConditionModel base_condition(void)
{
    return (MeasureConditionModel){
        .measure_condition_sid = copy_literal("789"),
        .condition_code = copy_literal("X"),
        .condition_duty_amount = copy_literal("12.5"),
        .condition_monetary_unit_code = copy_literal("EUC"),
        .condition_measurement_unit_code = copy_literal("KGM"),
        .certificate_type_code = copy_literal("C"),
        .certificate_code = copy_literal("990"),
        .action_code = copy_literal("01"),
        .measure_condition_code_description = copy_literal("Other conditions"),
        .certificate_type_description = copy_literal("Certificate"),
        .certificate_description = copy_literal("Import certificate"),
        .measurement_unit_description = copy_literal("Kilogram"),
    };
}

static MeasureConditionComponentModel component(const char *duty_expression_id)
{
    return (MeasureConditionComponentModel){
        .measure_condition_sid = copy_literal("789"),
        .duty_expression_id = copy_literal(duty_expression_id),
        .duty_amount = copy_literal("8.95"),
        .monetary_unit_code = copy_literal("EUC"),
        .measurement_unit_code = copy_literal("KGM"),
    };
}

static Appendix5aModel appendix_5a(void)
{
    return (Appendix5aModel){
        .certificate_type_code = copy_literal("C"),
        .certificate_code = copy_literal("990"),
        .cds_guidance = copy_literal("CDS guidance text"),
    };
}

static void renders_named_condition_concepts(void **state)
{
    (void)state;

    MeasureConditionModel model = base_condition();
    char *document_code = measure_condition_presenter_document_code(&model);
    char *condition = measure_condition_presenter_condition(&model);
    char *requirement = measure_condition_presenter_requirement(&model);

    assert_string_equal(measure_condition_presenter_resource_type(), "measure_condition");
    assert_string_equal(measure_condition_presenter_id(&model), "789");
    assert_string_equal(document_code, "C990");
    assert_string_equal(condition, "X: Other conditions");
    assert_string_equal(requirement, "Certificate: Import certificate");
    assert_string_equal(measure_condition_presenter_measure_condition_class(&model), "threshold");
    assert_string_equal(measure_condition_presenter_requirement_operator(&model), ">");
    assert_string_equal(measure_condition_presenter_threshold_unit_type(&model), "price");
    assert_string_equal(measure_condition_presenter_monetary_unit_abbreviation(&model), "EUR (EUC)");

    free(document_code);
    free(condition);
    free(requirement);
    measure_condition_model_free(&model);
}

static void returns_appendix_5a_guidance_for_document_code(void **state)
{
    (void)state;

    MeasureConditionModel model = base_condition();
    Appendix5aModelList appendix_5as = { 0 };
    assert_true(appendix_5a_model_list_append(&appendix_5as, appendix_5a()));

    assert_string_equal(measure_condition_presenter_guidance_cds(&model, &appendix_5as), "CDS guidance text");

    appendix_5a_model_list_free(&appendix_5as);
    measure_condition_model_free(&model);
}

static void classifies_document_negative_and_exemption_conditions(void **state)
{
    (void)state;

    MeasureConditionModel document = base_condition();
    free(document.condition_duty_amount);
    document.condition_duty_amount = NULL;
    assert_string_equal(measure_condition_presenter_measure_condition_class(&document), "document");

    free(document.action_code);
    document.action_code = copy_literal("04");
    assert_string_equal(measure_condition_presenter_measure_condition_class(&document), "negative");

    free(document.action_code);
    document.action_code = copy_literal("01");
    free(document.certificate_type_code);
    document.certificate_type_code = copy_literal("Y");
    assert_string_equal(measure_condition_presenter_measure_condition_class(&document), "exemption");

    measure_condition_model_free(&document);
}

static void renders_duty_requirement_without_document(void **state)
{
    (void)state;

    MeasureConditionModel model = base_condition();
    free(model.certificate_code);
    model.certificate_code = NULL;

    char *requirement = measure_condition_presenter_requirement(&model);

    assert_string_equal(requirement, "<span>12.5</span> EUR (EUC) / <abbr title='Kilogram'>Kilogram</abbr>");

    free(requirement);
    measure_condition_model_free(&model);
}

static void returns_null_abbreviation_for_unmapped_monetary_unit(void **state)
{
    (void)state;

    MeasureConditionModel model = base_condition();
    free(model.condition_monetary_unit_code);
    model.condition_monetary_unit_code = copy_literal("GBP");

    assert_null(measure_condition_presenter_monetary_unit_abbreviation(&model));

    measure_condition_model_free(&model);
}

static void renders_component_duty_expression_for_condition(void **state)
{
    (void)state;

    MeasureConditionModel model = base_condition();
    MeasureConditionComponentModelList components = { 0 };
    assert_true(measure_condition_component_model_list_append(&components, component("01")));

    char *duty_expression = measure_condition_presenter_duty_expression(&model, &components);

    assert_string_equal(duty_expression, "<span>8.95</span> % EUR (EUC) / KGM");

    free(duty_expression);
    measure_condition_component_model_list_free(&components);
    measure_condition_model_free(&model);
}

static void classifies_threshold_units_in_rails_priority_order(void **state)
{
    (void)state;

    MeasureConditionModel model = base_condition();
    assert_string_equal(measure_condition_presenter_threshold_unit_type(&model), "price");

    free(model.condition_monetary_unit_code);
    model.condition_monetary_unit_code = NULL;
    assert_string_equal(measure_condition_presenter_threshold_unit_type(&model), "weight");

    free(model.condition_measurement_unit_code);
    model.condition_measurement_unit_code = copy_literal("LTR");
    assert_string_equal(measure_condition_presenter_threshold_unit_type(&model), "volume");

    free(model.condition_measurement_unit_code);
    model.condition_measurement_unit_code = copy_literal("ASV");
    assert_string_equal(measure_condition_presenter_threshold_unit_type(&model), "percentage_abv");

    free(model.condition_monetary_unit_code);
    model.condition_monetary_unit_code = copy_literal("EUC");
    free(model.condition_measurement_unit_code);
    model.condition_measurement_unit_code = copy_literal("KGM");
    free(model.condition_code);
    model.condition_code = copy_literal("V");
    assert_string_equal(measure_condition_presenter_threshold_unit_type(&model), "eps");

    measure_condition_model_free(&model);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(renders_named_condition_concepts),
        cmocka_unit_test(returns_appendix_5a_guidance_for_document_code),
        cmocka_unit_test(classifies_document_negative_and_exemption_conditions),
        cmocka_unit_test(renders_duty_requirement_without_document),
        cmocka_unit_test(returns_null_abbreviation_for_unmapped_monetary_unit),
        cmocka_unit_test(renders_component_duty_expression_for_condition),
        cmocka_unit_test(classifies_threshold_units_in_rails_priority_order),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
