#include "renderers/additional_code_jsonapi_renderer.h"

#include "presenters/additional_code_presenter.h"
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

yyjson_mut_val *additional_code_jsonapi_resource(yyjson_mut_doc *doc,
                                                 const AdditionalCodeModel *additional_code)
{
    if (doc == NULL || additional_code == NULL || additional_code->additional_code_sid == NULL) {
        return NULL;
    }

    yyjson_mut_val *resource = jsonapi_resource_object(doc,
                                                       additional_code_presenter_resource_type(),
                                                       additional_code_presenter_id(additional_code));
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    char *code = additional_code_presenter_code(additional_code);
    char *formatted_description = additional_code_presenter_description(additional_code, 1);
    if (resource == NULL || attributes == NULL || code == NULL || formatted_description == NULL ||
        add_string_or_null(doc, attributes, "code", code) == 0 ||
        add_string_or_null(doc, attributes, "description", additional_code->description) == 0 ||
        add_string_or_null(doc, attributes, "formatted_description", formatted_description) == 0 ||
        !yyjson_mut_obj_add_val(doc, resource, "attributes", attributes)) {
        free(code);
        free(formatted_description);
        return NULL;
    }

    free(code);
    free(formatted_description);
    return resource;
}

int additional_code_jsonapi_renderer_self_test(void)
{
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    AdditionalCodeModel model = {
        .additional_code_sid = "123",
        .additional_code_type_id = "8",
        .additional_code = "123",
        .description = "Additional duty",
    };
    yyjson_mut_val *resource = additional_code_jsonapi_resource(doc, &model);
    int ok = resource != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
