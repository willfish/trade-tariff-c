#include "renderers/measure_legal_act_jsonapi_renderer.h"

#include "presenters/measure_legal_act_presenter.h"
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

yyjson_mut_val *measure_legal_act_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                              const MeasureLegalActModel *legal_act)
{
    const char *id = measure_legal_act_presenter_id(legal_act);
    if (doc == NULL || id == NULL) {
        return NULL;
    }

    return jsonapi_resource_identifier(doc, measure_legal_act_presenter_resource_type(), id);
}

yyjson_mut_val *measure_legal_act_jsonapi_relationship_data_for_measure(
    yyjson_mut_doc *doc,
    const MeasureLegalActModelList *legal_acts,
    const char *measure_sid)
{
    if (doc == NULL || legal_acts == NULL || measure_sid == NULL) {
        return NULL;
    }

    yyjson_mut_val *data = yyjson_mut_arr(doc);
    if (data == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < legal_acts->len; i++) {
        const MeasureLegalActModel *legal_act = &legal_acts->items[i];
        if (!streq(legal_act->measure_sid, measure_sid)) {
            continue;
        }
        yyjson_mut_val *ref = measure_legal_act_jsonapi_resource_identifier(doc, legal_act);
        if (ref == NULL || !yyjson_mut_arr_append(data, ref)) {
            return NULL;
        }
    }

    return data;
}

yyjson_mut_val *measure_legal_act_jsonapi_suspension_relationship(
    yyjson_mut_doc *doc,
    const MeasureLegalActModelList *legal_acts,
    const char *measure_sid)
{
    const MeasureLegalActModel *suspension = measure_legal_act_presenter_suspension_for_measure(legal_acts, measure_sid);
    return jsonapi_nullable_relationship(doc,
                                         measure_legal_act_presenter_suspension_resource_type(),
                                         measure_legal_act_presenter_id(suspension));
}

yyjson_mut_val *measure_legal_act_jsonapi_resource(yyjson_mut_doc *doc,
                                                   const MeasureLegalActModel *legal_act,
                                                   const char *service)
{
    (void)service;
    const char *id = measure_legal_act_presenter_id(legal_act);
    if (doc == NULL || id == NULL) {
        return NULL;
    }

    yyjson_mut_val *resource = jsonapi_resource_object(doc,
                                                       measure_legal_act_presenter_resource_type(),
                                                       id);
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    char *code = measure_legal_act_presenter_suspension_regulation_code(legal_act);
    char *url = measure_legal_act_presenter_suspension_regulation_url(legal_act);
    const char *description = measure_legal_act_presenter_description(legal_act);
    if (resource == NULL || attributes == NULL || code == NULL || url == NULL ||
        add_string_or_null(doc, attributes, "validity_start_date", legal_act->validity_start_date) == 0 ||
        add_string_or_null(doc, attributes, "validity_end_date", legal_act->validity_end_date) == 0 ||
        add_string_or_null(doc, attributes, "officialjournal_number", legal_act->officialjournal_number) == 0 ||
        add_string_or_null(doc, attributes, "officialjournal_page", legal_act->officialjournal_page) == 0 ||
        add_string_or_null(doc, attributes, "published_date", legal_act->published_date) == 0 ||
        add_string_or_null(doc, attributes, "regulation_code", code) == 0 ||
        add_string_or_null(doc, attributes, "regulation_url", url) == 0 ||
        add_string_or_null(doc, attributes, "description", description) == 0 ||
        add_string_or_null(doc, attributes, "role", legal_act->role) == 0 ||
        !yyjson_mut_obj_add_val(doc, resource, "attributes", attributes)) {
        free(code);
        free(url);
        return NULL;
    }

    free(code);
    free(url);
    return resource;
}

yyjson_mut_val *measure_legal_act_jsonapi_suspension_resource(yyjson_mut_doc *doc,
                                                              const MeasureLegalActModel *legal_act)
{
    const char *id = measure_legal_act_presenter_id(legal_act);
    if (doc == NULL || id == NULL) {
        return NULL;
    }

    yyjson_mut_val *resource = jsonapi_resource_object(doc,
                                                       measure_legal_act_presenter_suspension_resource_type(),
                                                       id);
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    char *code = measure_legal_act_presenter_regulation_code(legal_act);
    char *url = measure_legal_act_presenter_regulation_url(legal_act);
    if (resource == NULL || attributes == NULL || code == NULL || url == NULL ||
        add_string_or_null(doc, attributes, "validity_end_date", legal_act->effective_end_date) == 0 ||
        add_string_or_null(doc, attributes, "validity_start_date", legal_act->effective_start_date) == 0 ||
        add_string_or_null(doc, attributes, "regulation_code", code) == 0 ||
        add_string_or_null(doc, attributes, "regulation_url", url) == 0 ||
        !yyjson_mut_obj_add_val(doc, resource, "attributes", attributes)) {
        free(code);
        free(url);
        return NULL;
    }

    free(code);
    free(url);
    return resource;
}

int measure_legal_act_jsonapi_renderer_self_test(void)
{
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    MeasureLegalActModel model = {
        .measure_sid = "123",
        .measure_type_id = "103",
        .regulation_id = "R230001",
        .role = "1",
        .source = "generating",
        .validity_start_date = "2024-01-01T00:00:00.000Z",
        .officialjournal_number = "L1",
        .officialjournal_page = "10",
        .published_date = "2024-01-02",
        .information_text = "Regulation text",
    };
    yyjson_mut_val *resource = measure_legal_act_jsonapi_resource(doc, &model, "uk");
    MeasureLegalActModel suspension = {
        .measure_sid = "123",
        .measure_type_id = "103",
        .regulation_id = "R230002",
        .source = "suspension",
        .effective_start_date = "2024-01-01",
    };
    int ok = resource != NULL &&
             measure_legal_act_jsonapi_suspension_relationship(doc, &(MeasureLegalActModelList){ .items = &suspension, .len = 1 }, "123") != NULL &&
             measure_legal_act_jsonapi_suspension_resource(doc, &suspension) != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
