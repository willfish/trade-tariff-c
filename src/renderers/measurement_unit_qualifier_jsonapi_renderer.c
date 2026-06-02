#include "renderers/measurement_unit_qualifier_jsonapi_renderer.h"

#include "presenters/measurement_unit_qualifier_presenter.h"
#include "renderers/jsonapi_relationship.h"

#include <stdlib.h>

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

yyjson_mut_val *measurement_unit_qualifier_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                                       const char *measurement_unit_qualifier_code)
{
    if (doc == NULL || measurement_unit_qualifier_code == NULL) {
        return NULL;
    }

    return jsonapi_resource_identifier(doc,
                                       measurement_unit_qualifier_presenter_resource_type(),
                                       measurement_unit_qualifier_code);
}

yyjson_mut_val *measurement_unit_qualifier_jsonapi_resource(yyjson_mut_doc *doc,
                                                            const MeasurementUnitQualifierModel *qualifier,
                                                            const char *service)
{
    if (doc == NULL || qualifier == NULL || qualifier->measurement_unit_qualifier_code == NULL) {
        return NULL;
    }

    char *formatted = measurement_unit_qualifier_presenter_formatted_description(qualifier, service);
    const char *id = measurement_unit_qualifier_presenter_id(qualifier);
    yyjson_mut_val *resource = jsonapi_resource_object(doc,
                                                       measurement_unit_qualifier_presenter_resource_type(),
                                                       id);
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    if (formatted == NULL ||
        resource == NULL ||
        attributes == NULL ||
        !add_string_or_null(doc, attributes, "description", qualifier->description) ||
        !add_string_or_null(doc, attributes, "formatted_description", formatted) ||
        !yyjson_mut_obj_add_val(doc, resource, "attributes", attributes)) {
        free(formatted);
        return NULL;
    }

    free(formatted);
    return resource;
}

int measurement_unit_qualifier_jsonapi_renderer_self_test(void)
{
    MeasurementUnitQualifierModel qualifier = {
        .measurement_unit_qualifier_code = "X",
        .description = "per hectolitre",
    };
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val *resource = measurement_unit_qualifier_jsonapi_resource(doc, &qualifier, "uk");
    int ok = resource != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
