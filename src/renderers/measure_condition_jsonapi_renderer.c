#include "renderers/measure_condition_jsonapi_renderer.h"

#include "presenters/measure_condition_presenter.h"
#include "renderers/jsonapi_relationship.h"
#include "renderers/jsonapi_attribute.h"
#include "renderers/measure_condition_code_jsonapi_renderer.h"
#include "renderers/measure_condition_component_jsonapi_renderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int streq(const char *a, const char *b)
{
    return a != NULL && b != NULL && strcmp(a, b) == 0;
}

yyjson_mut_val *measure_condition_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                              const MeasureConditionModel *condition)
{
    const char *id = measure_condition_presenter_id(condition);
    if (doc == NULL || id == NULL) {
        return NULL;
    }
    return jsonapi_resource_identifier(doc, measure_condition_presenter_resource_type(), id);
}

yyjson_mut_val *measure_condition_jsonapi_relationship_data_for_measure(yyjson_mut_doc *doc,
                                                                        const MeasureConditionModelList *conditions,
                                                                        const char *measure_sid)
{
    if (doc == NULL || conditions == NULL || measure_sid == NULL) {
        return NULL;
    }

    yyjson_mut_val *data = yyjson_mut_arr(doc);
    if (data == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < conditions->len; i++) {
        const MeasureConditionModel *condition = &conditions->items[i];
        if (!streq(condition->measure_sid, measure_sid)) {
            continue;
        }

        yyjson_mut_val *ref = measure_condition_jsonapi_resource_identifier(doc, condition);
        if (ref == NULL || !yyjson_mut_arr_append(data, ref)) {
            return NULL;
        }
    }

    return data;
}

static yyjson_mut_val *condition_attributes(yyjson_mut_doc *doc,
                                            const MeasureConditionModel *condition,
                                            const MeasureConditionComponentModelList *components,
                                            const Appendix5aModelList *appendix_5as)
{
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    if (attributes == NULL || condition == NULL) {
        return NULL;
    }

    char *doc_code = measure_condition_presenter_document_code(condition);
    char *condition_text = measure_condition_presenter_condition(condition);
    char *duty_expression = measure_condition_presenter_duty_expression(condition, components);
    char *requirement = measure_condition_presenter_requirement(condition);
    const char *guidance_cds = measure_condition_presenter_guidance_cds(condition, appendix_5as);
    const char *condition_class = measure_condition_presenter_measure_condition_class(condition);
    const char *requirement_operator = measure_condition_presenter_requirement_operator(condition);
    const char *threshold_unit_type = measure_condition_presenter_threshold_unit_type(condition);
    const char *monetary_unit_abbreviation = measure_condition_presenter_monetary_unit_abbreviation(condition);

    int ok =
        jsonapi_add_string_or_null(doc, attributes, "action", condition->action) &&
        jsonapi_add_string_or_null(doc, attributes, "action_code", condition->action_code) &&
        jsonapi_add_string_or_null(doc, attributes, "certificate_description", condition->certificate_description) &&
        jsonapi_add_string_or_null(doc, attributes, "condition", condition_text) &&
        jsonapi_add_string_or_null(doc, attributes, "condition_code", condition->condition_code) &&
        jsonapi_add_number_string_or_null(doc, attributes, "condition_duty_amount", condition->condition_duty_amount) &&
        jsonapi_add_string_or_null(doc,
                                   attributes,
                                   "condition_measurement_unit_code",
                                   condition->condition_measurement_unit_code) &&
        jsonapi_add_string_or_null(doc,
                                   attributes,
                                   "condition_measurement_unit_qualifier_code",
                                   condition->condition_measurement_unit_qualifier_code) &&
        jsonapi_add_string_or_null(doc, attributes, "condition_monetary_unit_code", condition->condition_monetary_unit_code) &&
        jsonapi_add_string_or_null(doc, attributes, "document_code", doc_code) &&
        jsonapi_add_string_or_null(doc, attributes, "duty_expression", duty_expression) &&
        jsonapi_add_string_or_null(doc, attributes, "guidance_cds", guidance_cds) &&
        jsonapi_add_string_or_null(doc, attributes, "measure_condition_class", condition_class) &&
        jsonapi_add_string_or_null(doc, attributes, "monetary_unit_abbreviation", monetary_unit_abbreviation) &&
        jsonapi_add_string_or_null(doc, attributes, "requirement", requirement) &&
        jsonapi_add_string_or_null(doc, attributes, "requirement_operator", requirement_operator) &&
        jsonapi_add_string_or_null(doc, attributes, "threshold_unit_type", threshold_unit_type);

    free(doc_code);
    free(condition_text);
    free(duty_expression);
    free(requirement);
    return ok ? attributes : NULL;
}

static yyjson_mut_val *condition_relationships(yyjson_mut_doc *doc,
                                               const MeasureConditionModel *condition,
                                               const MeasureConditionComponentModelList *components)
{
    yyjson_mut_val *relationships = yyjson_mut_obj(doc);
    yyjson_mut_val *condition_code_data = measure_condition_code_jsonapi_resource_identifier(
        doc,
        condition == NULL ? NULL : condition->condition_code);
    yyjson_mut_val *component_data = measure_condition_component_jsonapi_relationship_data_for_condition(
        doc,
        components,
        condition == NULL ? NULL : condition->measure_condition_sid);
    if (relationships == NULL ||
        !jsonapi_add_data_relationship(doc, relationships, "measure_condition_code", condition_code_data) ||
        !jsonapi_add_data_relationship(doc, relationships, "measure_condition_components", component_data)) {
        return NULL;
    }
    return relationships;
}

yyjson_mut_val *measure_condition_jsonapi_resource(yyjson_mut_doc *doc,
                                                   const MeasureConditionModel *condition,
                                                   const MeasureConditionComponentModelList *components,
                                                   const Appendix5aModelList *appendix_5as)
{
    if (doc == NULL || condition == NULL || condition->measure_condition_sid == NULL) {
        return NULL;
    }

    const char *id = measure_condition_presenter_id(condition);
    if (id == NULL) {
        return NULL;
    }

    yyjson_mut_val *resource = jsonapi_resource_object(doc, measure_condition_presenter_resource_type(), id);
    yyjson_mut_val *attributes = condition_attributes(doc, condition, components, appendix_5as);
    yyjson_mut_val *relationships = condition_relationships(doc, condition, components);
    if (resource == NULL ||
        attributes == NULL ||
        relationships == NULL ||
        !yyjson_mut_obj_add_val(doc, resource, "attributes", attributes) ||
        !yyjson_mut_obj_add_val(doc, resource, "relationships", relationships)) {
        return NULL;
    }

    return resource;
}

int measure_condition_jsonapi_renderer_self_test(void)
{
    MeasureConditionModel condition = {
        .measure_condition_sid = "789",
        .measure_sid = "123",
        .condition_code = "B",
    };
    MeasureConditionComponentModelList components = { 0 };
    Appendix5aModelList appendix_5as = { 0 };
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val *resource = measure_condition_jsonapi_resource(doc, &condition, &components, &appendix_5as);
    int ok = resource != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
