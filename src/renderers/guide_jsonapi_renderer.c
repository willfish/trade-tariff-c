#include "renderers/guide_jsonapi_renderer.h"

#include "presenters/guide_presenter.h"
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

yyjson_mut_val *guide_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                  const GuideModel *guide)
{
    if (doc == NULL || guide == NULL || guide->id == NULL) {
        return NULL;
    }
    const char *id = guide_presenter_id(guide);

    return jsonapi_resource_identifier(doc, guide_presenter_resource_type(), id);
}

yyjson_mut_val *guide_jsonapi_relationship_data(yyjson_mut_doc *doc,
                                                const GuideModelList *guides)
{
    yyjson_mut_val *data = yyjson_mut_arr(doc);
    if (data == NULL || guides == NULL) {
        return data;
    }

    for (size_t i = 0; i < guides->len; i++) {
        yyjson_mut_val *ref = guide_jsonapi_resource_identifier(doc, &guides->items[i]);
        if (ref == NULL || !yyjson_mut_arr_append(data, ref)) {
            return NULL;
        }
    }
    return data;
}

yyjson_mut_val *guide_jsonapi_resource(yyjson_mut_doc *doc,
                                       const GuideModel *guide)
{
    if (doc == NULL || guide == NULL || guide->id == NULL) {
        return NULL;
    }
    const char *id = guide_presenter_id(guide);

    yyjson_mut_val *resource = jsonapi_resource_object(doc, guide_presenter_resource_type(), id);
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    if (resource == NULL ||
        attributes == NULL ||
        !add_string_or_null(doc, attributes, "title", guide->title) ||
        !add_string_or_null(doc, attributes, "url", guide->url) ||
        !yyjson_mut_obj_add_val(doc, resource, "attributes", attributes)) {
        return NULL;
    }

    return resource;
}

int guide_jsonapi_renderer_self_test(void)
{
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    GuideModel guide = {
        .id = "1",
        .title = "Guide title",
        .url = "https://example.test/guide",
    };
    int ok = doc != NULL &&
             guide_jsonapi_resource_identifier(doc, &guide) != NULL &&
             guide_jsonapi_resource(doc, &guide) != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
