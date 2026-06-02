#include "renderers/measure_type_jsonapi_renderer.h"

#include "presenters/measure_type_presenter.h"
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

static int add_presented_code(yyjson_mut_doc *doc,
                              yyjson_mut_val *obj,
                              const char *key,
                              MeasureTypePresentedCode code)
{
    if (code.is_null) {
        return yyjson_mut_obj_add_null(doc, obj, key);
    }
    if (code.is_int) {
        return yyjson_mut_obj_add_int(doc, obj, key, code.int_value);
    }
    return yyjson_mut_obj_add_strcpy(doc, obj, key, code.string_value);
}

yyjson_mut_val *measure_type_jsonapi_resource(yyjson_mut_doc *doc,
                                              const MeasureTypeModel *measure_type)
{
    const char *id = measure_type_presenter_id(measure_type);
    if (doc == NULL || id == NULL) {
        return NULL;
    }

    yyjson_mut_val *resource = jsonapi_resource_object(doc, measure_type_presenter_resource_type(), id);
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    int ok = resource != NULL &&
             attributes != NULL &&
             yyjson_mut_obj_add_strcpy(doc, attributes, "id", id) &&
             add_string_or_null(doc, attributes, "description", measure_type->description) &&
             add_string_or_null(doc, attributes, "measure_type_series_id", measure_type->measure_type_series_id) &&
             add_string_or_null(doc, attributes, "measure_type_series_description", measure_type->measure_type_series_description) &&
             add_presented_code(doc,
                                attributes,
                                "measure_component_applicable_code",
                                measure_type_presenter_code(measure_type->measure_component_applicable_code)) &&
             add_presented_code(doc,
                                attributes,
                                "order_number_capture_code",
                                measure_type_presenter_code(measure_type->order_number_capture_code)) &&
             add_presented_code(doc,
                                attributes,
                                "trade_movement_code",
                                measure_type_presenter_code(measure_type->trade_movement_code)) &&
             add_string_or_null(doc, attributes, "validity_start_date", measure_type->validity_start_date) &&
             add_string_or_null(doc, attributes, "validity_end_date", measure_type->validity_end_date) &&
             yyjson_mut_obj_add_val(doc, resource, "attributes", attributes);

    return ok ? resource : NULL;
}

int measure_type_jsonapi_renderer_self_test(void)
{
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    MeasureTypeModel model = {
        .measure_type_id = "103",
        .description = "Third country duty",
        .measure_type_series_id = "C",
        .measure_component_applicable_code = "1",
        .order_number_capture_code = "2",
        .trade_movement_code = "0",
    };
    yyjson_mut_val *resource = measure_type_jsonapi_resource(doc, &model);
    int ok = resource != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
