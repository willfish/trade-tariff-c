#include "renderers/jsonapi_relationship.h"

yyjson_mut_val *jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                            const char *type,
                                            const char *id)
{
    return jsonapi_resource_object(doc, type, id);
}

yyjson_mut_val *jsonapi_resource_object(yyjson_mut_doc *doc,
                                        const char *type,
                                        const char *id)
{
    if (doc == NULL || type == NULL || id == NULL) {
        return NULL;
    }

    yyjson_mut_val *resource = yyjson_mut_obj(doc);
    if (resource == NULL ||
        !yyjson_mut_obj_add_strcpy(doc, resource, "id", id) ||
        !yyjson_mut_obj_add_strcpy(doc, resource, "type", type)) {
        return NULL;
    }
    return resource;
}

yyjson_mut_val *jsonapi_data_relationship(yyjson_mut_doc *doc,
                                          yyjson_mut_val *data)
{
    if (doc == NULL || data == NULL) {
        return NULL;
    }

    yyjson_mut_val *relationship = yyjson_mut_obj(doc);
    if (relationship == NULL ||
        !yyjson_mut_obj_add_val(doc, relationship, "data", data)) {
        return NULL;
    }
    return relationship;
}

int jsonapi_add_data_relationship(yyjson_mut_doc *doc,
                                  yyjson_mut_val *relationships,
                                  const char *name,
                                  yyjson_mut_val *data)
{
    if (doc == NULL || relationships == NULL || name == NULL || data == NULL) {
        return 0;
    }

    yyjson_mut_val *relationship = jsonapi_data_relationship(doc, data);
    return relationship != NULL &&
           yyjson_mut_obj_add_val(doc, relationships, name, relationship);
}

yyjson_mut_val *jsonapi_empty_array_relationship(yyjson_mut_doc *doc)
{
    if (doc == NULL) {
        return NULL;
    }

    yyjson_mut_val *data = yyjson_mut_arr(doc);
    return jsonapi_data_relationship(doc, data);
}

yyjson_mut_val *jsonapi_single_relationship(yyjson_mut_doc *doc,
                                            const char *type,
                                            const char *id)
{
    yyjson_mut_val *data = jsonapi_resource_identifier(doc, type, id);
    return jsonapi_data_relationship(doc, data);
}

yyjson_mut_val *jsonapi_nullable_relationship(yyjson_mut_doc *doc,
                                              const char *type,
                                              const char *id)
{
    if (doc == NULL) {
        return NULL;
    }

    yyjson_mut_val *data = id == NULL ? yyjson_mut_null(doc) : jsonapi_resource_identifier(doc, type, id);
    return jsonapi_data_relationship(doc, data);
}

int jsonapi_relationship_self_test(void)
{
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val *resource = jsonapi_resource_object(doc, "commodity", "0101210000-80");
    yyjson_mut_val *single = jsonapi_single_relationship(doc, "measure", "123");
    yyjson_mut_val *nullable = jsonapi_nullable_relationship(doc, "order_number", NULL);
    yyjson_mut_val *empty = jsonapi_empty_array_relationship(doc);
    int ok = doc != NULL &&
             resource != NULL &&
             single != NULL &&
             nullable != NULL &&
             empty != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
