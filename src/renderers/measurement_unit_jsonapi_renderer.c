#include "renderers/measurement_unit_jsonapi_renderer.h"

#include "presenters/measurement_unit_presenter.h"
#include "renderers/jsonapi_relationship.h"

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

yyjson_mut_val *measurement_unit_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                             const char *measurement_unit_code)
{
    if (doc == NULL || measurement_unit_code == NULL) {
        return NULL;
    }

    return jsonapi_resource_identifier(doc,
                                       measurement_unit_presenter_resource_type(),
                                       measurement_unit_code);
}

yyjson_mut_val *measurement_unit_jsonapi_resource(yyjson_mut_doc *doc,
                                                  const MeasurementUnitModel *unit)
{
    if (doc == NULL || unit == NULL || unit->measurement_unit_code == NULL) {
        return NULL;
    }

    const char *id = measurement_unit_presenter_id(unit);
    yyjson_mut_val *resource = jsonapi_resource_object(doc, measurement_unit_presenter_resource_type(), id);
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    if (resource == NULL ||
        attributes == NULL ||
        !add_string_or_null(doc, attributes, "description", unit->description) ||
        !yyjson_mut_obj_add_strcpy(doc, attributes, "measurement_unit_code", unit->measurement_unit_code) ||
        !yyjson_mut_obj_add_val(doc, resource, "attributes", attributes)) {
        return NULL;
    }

    return resource;
}

int measurement_unit_jsonapi_renderer_self_test(void)
{
    MeasurementUnitModel unit = {
        .measurement_unit_code = "KGM",
        .description = "Kilogram",
        .abbreviation = "kg",
    };
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val *resource = measurement_unit_jsonapi_resource(doc, &unit);
    int ok = resource != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
