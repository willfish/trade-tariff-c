#include "renderers/measure_condition_code_jsonapi_renderer.h"

#include "presenters/measure_condition_code_presenter.h"
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

yyjson_mut_val *measure_condition_code_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                                   const char *condition_code)
{
    if (doc == NULL || condition_code == NULL) {
        return NULL;
    }

    return jsonapi_resource_identifier(doc,
                                       measure_condition_code_presenter_resource_type(),
                                       condition_code);
}

yyjson_mut_val *measure_condition_code_jsonapi_resource(yyjson_mut_doc *doc,
                                                       const MeasureConditionCodeModel *condition_code)
{
    if (doc == NULL || condition_code == NULL || condition_code->condition_code == NULL) {
        return NULL;
    }

    const char *id = measure_condition_code_presenter_id(condition_code);
    yyjson_mut_val *resource = jsonapi_resource_object(doc,
                                                       measure_condition_code_presenter_resource_type(),
                                                       id);
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    int ok = resource != NULL &&
             attributes != NULL &&
             id != NULL &&
             add_string_or_null(doc, attributes, "description", condition_code->description) &&
             add_string_or_null(doc, attributes, "validity_start_date", condition_code->validity_start_date) &&
             add_string_or_null(doc, attributes, "validity_end_date", condition_code->validity_end_date) &&
             yyjson_mut_obj_add_val(doc, resource, "attributes", attributes);

    return ok ? resource : NULL;
}

int measure_condition_code_jsonapi_renderer_self_test(void)
{
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    MeasureConditionCodeModel model = {
        .condition_code = "B",
        .description = "Presentation of a certificate/licence/document",
    };
    yyjson_mut_val *resource = measure_condition_code_jsonapi_resource(doc, &model);
    int ok = resource != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
