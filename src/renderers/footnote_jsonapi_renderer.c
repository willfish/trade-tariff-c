#include "renderers/footnote_jsonapi_renderer.h"

#include "presenters/footnote_presenter.h"
#include "renderers/jsonapi_relationship.h"

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

yyjson_mut_val *footnote_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                     const Footnote *footnote)
{
    const char *id = footnote_presenter_id(footnote);
    if (doc == NULL || id == NULL) {
        return NULL;
    }

    return jsonapi_resource_identifier(doc, footnote_presenter_resource_type(), id);
}

yyjson_mut_val *footnote_jsonapi_relationship_data_for_measure(yyjson_mut_doc *doc,
                                                               const FootnoteList *footnotes,
                                                               const char *measure_sid)
{
    if (doc == NULL || footnotes == NULL || measure_sid == NULL) {
        return NULL;
    }

    yyjson_mut_val *data = yyjson_mut_arr(doc);
    if (data == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < footnotes->len; i++) {
        const Footnote *footnote = &footnotes->items[i];
        if (!streq(footnote->measure_sid, measure_sid)) {
            continue;
        }
        yyjson_mut_val *ref = footnote_jsonapi_resource_identifier(doc, footnote);
        if (ref == NULL || !yyjson_mut_arr_append(data, ref)) {
            return NULL;
        }
    }

    return data;
}

yyjson_mut_val *footnote_jsonapi_resource(yyjson_mut_doc *doc,
                                          const Footnote *footnote,
                                          const char *service)
{
    const char *id = footnote_presenter_id(footnote);
    if (doc == NULL || id == NULL) {
        return NULL;
    }

    yyjson_mut_val *resource = jsonapi_resource_object(doc, footnote_presenter_resource_type(), id);
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    char *formatted_description = footnote_presenter_formatted_description(footnote, service);
    if (resource == NULL || attributes == NULL || formatted_description == NULL ||
        add_string_or_null(doc, attributes, "code", footnote_presenter_code(footnote)) == 0 ||
        add_string_or_null(doc, attributes, "description", formatted_description) == 0 ||
        add_string_or_null(doc, attributes, "formatted_description", formatted_description) == 0 ||
        !yyjson_mut_obj_add_val(doc, resource, "attributes", attributes)) {
        free(formatted_description);
        return NULL;
    }

    free(formatted_description);
    return resource;
}

int footnote_jsonapi_renderer_self_test(void)
{
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    Footnote footnote = {
        .measure_sid = "123",
        .id = "CD123",
        .code = "CD123",
        .description = "Footnote text",
    };
    yyjson_mut_val *resource = footnote_jsonapi_resource(doc, &footnote, "uk");
    int ok = resource != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
