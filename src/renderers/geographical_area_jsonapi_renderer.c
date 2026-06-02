#include "renderers/geographical_area_jsonapi_renderer.h"

#include "presenters/geographical_area_presenter.h"
#include "renderers/jsonapi_relationship.h"

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

yyjson_mut_val *geographical_area_jsonapi_resource_identifier(yyjson_mut_doc *doc,
                                                              const char *geographical_area_id)
{
    if (doc == NULL || geographical_area_id == NULL) {
        return NULL;
    }

    return jsonapi_resource_identifier(doc,
                                       geographical_area_presenter_resource_type(),
                                       geographical_area_id);
}

yyjson_mut_val *geographical_area_jsonapi_excluded_relationship_data_for_measure(
    yyjson_mut_doc *doc,
    const MeasureExcludedGeographicalAreaModelList *excluded,
    const char *measure_sid)
{
    if (doc == NULL || excluded == NULL || measure_sid == NULL) {
        return NULL;
    }

    yyjson_mut_val *data = yyjson_mut_arr(doc);
    if (data == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < excluded->len; i++) {
        const MeasureExcludedGeographicalAreaModel *item = &excluded->items[i];
        if (!streq(item->measure_sid, measure_sid)) {
            continue;
        }
        yyjson_mut_val *ref = geographical_area_jsonapi_resource_identifier(
            doc,
            geographical_area_presenter_excluded_id(item));
        if (ref == NULL || !yyjson_mut_arr_append(data, ref)) {
            return NULL;
        }
    }

    return data;
}

static yyjson_mut_val *children_relationship(yyjson_mut_doc *doc,
                                             const GeographicalAreaModel *area,
                                             const GeographicalAreaModelList *all_areas)
{
    yyjson_mut_val *relationship = yyjson_mut_obj(doc);
    yyjson_mut_val *data = yyjson_mut_arr(doc);
    if (relationship == NULL || data == NULL || area == NULL || all_areas == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < all_areas->len; i++) {
        const GeographicalAreaModel *candidate = &all_areas->items[i];
        if (!streq(candidate->parent_geographical_area_sid, area->geographical_area_sid)) {
            continue;
        }
        yyjson_mut_val *ref = geographical_area_jsonapi_resource_identifier(
            doc,
            geographical_area_presenter_id(candidate));
        if (ref == NULL || !yyjson_mut_arr_append(data, ref)) {
            return NULL;
        }
    }

    return yyjson_mut_obj_add_val(doc, relationship, "data", data) ? relationship : NULL;
}

yyjson_mut_val *geographical_area_jsonapi_resource(yyjson_mut_doc *doc,
                                                   const GeographicalAreaModel *area,
                                                   const GeographicalAreaModelList *all_areas)
{
    const char *id = geographical_area_presenter_id(area);
    if (doc == NULL || id == NULL) {
        return NULL;
    }

    yyjson_mut_val *resource = jsonapi_resource_object(doc,
                                                       geographical_area_presenter_resource_type(),
                                                       id);
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    yyjson_mut_val *relationships = yyjson_mut_obj(doc);
    yyjson_mut_val *children = children_relationship(doc, area, all_areas);
    if (resource == NULL ||
        attributes == NULL ||
        relationships == NULL ||
        children == NULL ||
        add_string_or_null(doc, attributes, "id", id) == 0 ||
        add_string_or_null(doc, attributes, "description", area->description) == 0 ||
        add_string_or_null(doc, attributes, "geographical_area_id", id) == 0 ||
        add_string_or_null(doc, attributes, "geographical_area_sid", area->geographical_area_sid) == 0 ||
        !yyjson_mut_obj_add_val(doc, relationships, "children_geographical_areas", children) ||
        !yyjson_mut_obj_add_val(doc, resource, "attributes", attributes) ||
        !yyjson_mut_obj_add_val(doc, resource, "relationships", relationships)) {
        return NULL;
    }

    return resource;
}

int geographical_area_jsonapi_renderer_self_test(void)
{
    GeographicalAreaModelList areas = { 0 };
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    GeographicalAreaModel area = {
        .geographical_area_sid = "100",
        .geographical_area_id = "1011",
        .description = "All countries",
    };
    yyjson_mut_val *resource = geographical_area_jsonapi_resource(doc, &area, &areas);
    int ok = resource != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
