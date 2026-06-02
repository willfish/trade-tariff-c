#include "renderers/measure_condition_component_jsonapi_renderer.h"

#include "presenters/measure_condition_component_presenter.h"
#include "presenters/monetary_unit_presenter.h"
#include "renderers/jsonapi_relationship.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

static int streq(const char *a, const char *b)
{
    return a != NULL && b != NULL && strcmp(a, b) == 0;
}

static int add_string_or_null(yyjson_mut_doc *doc,
                              yyjson_mut_val *obj,
                              const char *key,
                              const char *value)
{
    if (value == NULL) {
        return yyjson_mut_obj_add_null(doc, obj, key);
    }
    return yyjson_mut_obj_add_strcpy(doc, obj, key, value);
}

static int add_number_or_null(yyjson_mut_doc *doc,
                              yyjson_mut_val *obj,
                              const char *key,
                              const char *value)
{
    if (value == NULL) {
        return yyjson_mut_obj_add_null(doc, obj, key);
    }

    char *end = NULL;
    errno = 0;
    double number = strtod(value, &end);
    if (errno != 0 || end == value || end == NULL || *end != '\0') {
        return yyjson_mut_obj_add_strcpy(doc, obj, key, value);
    }
    return yyjson_mut_obj_add_real(doc, obj, key, number);
}

char *measure_condition_component_jsonapi_id(const MeasureConditionComponentModel *component)
{
    return measure_condition_component_presenter_id(component);
}

yyjson_mut_val *measure_condition_component_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                                        const MeasureConditionComponentModel *component)
{
    if (doc == NULL) {
        return NULL;
    }

    char *id = measure_condition_component_jsonapi_id(component);
    yyjson_mut_val *ref =
        id == NULL ? NULL : jsonapi_resource_identifier(doc, measure_condition_component_presenter_resource_type(), id);
    free(id);
    return ref;
}

yyjson_mut_val *measure_condition_component_jsonapi_relationship_data_for_condition(yyjson_mut_doc *doc,
                                                                                    const MeasureConditionComponentModelList *components,
                                                                                    const char *measure_condition_sid)
{
    if (doc == NULL || components == NULL || measure_condition_sid == NULL) {
        return NULL;
    }

    yyjson_mut_val *data = yyjson_mut_arr(doc);
    if (data == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < components->len; i++) {
        const MeasureConditionComponentModel *component = &components->items[i];
        if (!streq(component->measure_condition_sid, measure_condition_sid)) {
            continue;
        }

        yyjson_mut_val *ref = measure_condition_component_jsonapi_resource_identifier(doc, component);
        if (ref == NULL || !yyjson_mut_arr_append(data, ref)) {
            return NULL;
        }
    }

    return data;
}

static yyjson_mut_val *component_attributes(yyjson_mut_doc *doc,
                                            const MeasureConditionComponentModel *component)
{
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    if (attributes == NULL || component == NULL) {
        return NULL;
    }

    const char *abbreviation = measure_condition_component_presenter_duty_expression_abbreviation(component);
    const char *monetary_unit_abbreviation = monetary_unit_presenter_abbreviation_for_code(component->monetary_unit_code);
    int ok =
        add_string_or_null(doc, attributes, "duty_expression_id", component->duty_expression_id) &&
        add_number_or_null(doc, attributes, "duty_amount", component->duty_amount) &&
        add_string_or_null(doc, attributes, "monetary_unit_code", component->monetary_unit_code) &&
        add_string_or_null(doc, attributes, "monetary_unit_abbreviation", monetary_unit_abbreviation) &&
        add_string_or_null(doc, attributes, "measurement_unit_code", component->measurement_unit_code) &&
        add_string_or_null(doc, attributes, "measurement_unit_qualifier_code", component->measurement_unit_qualifier_code) &&
        add_string_or_null(doc, attributes, "duty_expression_description", component->duty_expression_description) &&
        add_string_or_null(doc, attributes, "duty_expression_abbreviation", abbreviation) &&
        add_string_or_null(doc, attributes, "measure_condition_sid", component->measure_condition_sid);

    return ok ? attributes : NULL;
}

yyjson_mut_val *measure_condition_component_jsonapi_resource(yyjson_mut_doc *doc,
                                                             const MeasureConditionComponentModel *component)
{
    if (doc == NULL || component == NULL) {
        return NULL;
    }

    char *id = measure_condition_component_jsonapi_id(component);
    yyjson_mut_val *resource = jsonapi_resource_object(doc,
                                                       measure_condition_component_presenter_resource_type(),
                                                       id);
    yyjson_mut_val *attributes = component_attributes(doc, component);
    if (id == NULL ||
        resource == NULL ||
        attributes == NULL ||
        !yyjson_mut_obj_add_val(doc, resource, "attributes", attributes)) {
        free(id);
        return NULL;
    }

    free(id);
    return resource;
}

int measure_condition_component_jsonapi_renderer_self_test(void)
{
    MeasureConditionComponentModel component = {
        .measure_condition_sid = "789",
        .duty_expression_id = "01",
        .duty_amount = "8.95",
    };
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val *resource = measure_condition_component_jsonapi_resource(doc, &component);
    int ok = resource != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
