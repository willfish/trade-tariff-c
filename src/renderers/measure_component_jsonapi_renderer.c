#include "renderers/measure_component_jsonapi_renderer.h"

#include "presenters/measure_component_presenter.h"
#include "presenters/measurement_unit_presenter.h"
#include "presenters/measurement_unit_qualifier_presenter.h"
#include "presenters/monetary_unit_presenter.h"
#include "renderers/jsonapi_attribute.h"
#include "renderers/jsonapi_relationship.h"

#include <stdlib.h>
#include <string.h>

static int streq(const char *a, const char *b)
{
    return a != NULL && b != NULL && strcmp(a, b) == 0;
}

char *measure_component_jsonapi_id(const MeasureComponentModel *component)
{
    return measure_component_presenter_id(component);
}

yyjson_mut_val *measure_component_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                              const MeasureComponentModel *component)
{
    if (doc == NULL) {
        return NULL;
    }

    char *id = measure_component_jsonapi_id(component);
    yyjson_mut_val *ref = id == NULL ? NULL : jsonapi_resource_identifier(doc, measure_component_presenter_resource_type(), id);
    free(id);
    return ref;
}

yyjson_mut_val *measure_component_jsonapi_relationship_data_for_measure(yyjson_mut_doc *doc,
                                                                        const MeasureComponentModelList *components,
                                                                        const char *measure_sid)
{
    if (doc == NULL || components == NULL || measure_sid == NULL) {
        return NULL;
    }

    yyjson_mut_val *data = yyjson_mut_arr(doc);
    if (data == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < components->len; i++) {
        const MeasureComponentModel *component = &components->items[i];
        const char *owner_measure_sid = component->resolved_for_measure_sid != NULL
                                            ? component->resolved_for_measure_sid
                                            : component->measure_sid;
        if (!streq(owner_measure_sid, measure_sid)) {
            continue;
        }

        yyjson_mut_val *ref = measure_component_jsonapi_resource_identifier(doc, component);
        if (ref == NULL || !yyjson_mut_arr_append(data, ref)) {
            return NULL;
        }
    }

    return data;
}

static yyjson_mut_val *component_attributes(yyjson_mut_doc *doc,
                                            const MeasureComponentModel *component)
{
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    if (attributes == NULL || component == NULL) {
        return NULL;
    }

    const char *abbreviation = measure_component_presenter_duty_expression_abbreviation(component);
    const char *monetary_unit_abbreviation = monetary_unit_presenter_abbreviation_for_code(component->monetary_unit_code);
    int ok =
        jsonapi_add_string_or_null(doc, attributes, "duty_expression_id", component->duty_expression_id) &&
        jsonapi_add_number_string_or_null(doc, attributes, "duty_amount", component->duty_amount) &&
        jsonapi_add_string_or_null(doc, attributes, "monetary_unit_code", component->monetary_unit_code) &&
        jsonapi_add_string_or_null(doc, attributes, "monetary_unit_abbreviation", monetary_unit_abbreviation) &&
        jsonapi_add_string_or_null(doc, attributes, "measurement_unit_code", component->measurement_unit_code) &&
        jsonapi_add_string_or_null(doc,
                                   attributes,
                                   "measurement_unit_qualifier_code",
                                   component->measurement_unit_qualifier_code) &&
        jsonapi_add_string_or_null(doc, attributes, "duty_expression_description", component->duty_expression_description) &&
        jsonapi_add_string_or_null(doc, attributes, "duty_expression_abbreviation", abbreviation);

    return ok ? attributes : NULL;
}

static yyjson_mut_val *component_relationships(yyjson_mut_doc *doc,
                                               const MeasureComponentModel *component)
{
    yyjson_mut_val *relationships = yyjson_mut_obj(doc);
    if (relationships == NULL || component == NULL) {
        return NULL;
    }

    if (component->measurement_unit_code == NULL) {
        return relationships;
    }

    yyjson_mut_val *measurement_unit = jsonapi_nullable_relationship(doc,
                                                                     measurement_unit_presenter_resource_type(),
                                                                     component->measurement_unit_code);
    yyjson_mut_val *measurement_unit_qualifier = jsonapi_nullable_relationship(doc,
                                                                               measurement_unit_qualifier_presenter_resource_type(),
                                                                               component->measurement_unit_qualifier_code);
    if (measurement_unit == NULL ||
        measurement_unit_qualifier == NULL ||
        !yyjson_mut_obj_add_val(doc, relationships, "measurement_unit", measurement_unit) ||
        !yyjson_mut_obj_add_val(doc, relationships, "measurement_unit_qualifier", measurement_unit_qualifier)) {
        return NULL;
    }

    return relationships;
}

yyjson_mut_val *measure_component_jsonapi_resource(yyjson_mut_doc *doc,
                                                   const MeasureComponentModel *component)
{
    if (doc == NULL || component == NULL) {
        return NULL;
    }

    char *id = measure_component_jsonapi_id(component);
    yyjson_mut_val *resource = jsonapi_resource_object(doc, measure_component_presenter_resource_type(), id);
    yyjson_mut_val *attributes = component_attributes(doc, component);
    yyjson_mut_val *relationships = component_relationships(doc, component);
    if (id == NULL ||
        resource == NULL ||
        attributes == NULL ||
        relationships == NULL ||
        !yyjson_mut_obj_add_val(doc, resource, "attributes", attributes) ||
        !yyjson_mut_obj_add_val(doc, resource, "relationships", relationships)) {
        free(id);
        return NULL;
    }

    free(id);
    return resource;
}

int measure_component_jsonapi_renderer_self_test(void)
{
    MeasureComponentModel component = {
        .measure_sid = "123",
        .duty_expression_id = "01",
        .duty_amount = "8.95",
        .monetary_unit_code = "EUC",
        .measurement_unit_code = "KGM",
    };
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val *resource = measure_component_jsonapi_resource(doc, &component);
    int ok = resource != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
