#include "renderers/measure_condition_permutation_jsonapi_renderer.h"

#include "presenters/measure_condition_permutation_presenter.h"
#include "presenters/measure_condition_presenter.h"
#include "renderers/jsonapi_relationship.h"

#include <string.h>

static int streq(const char *a, const char *b)
{
    return a != NULL && b != NULL && strcmp(a, b) == 0;
}

static yyjson_mut_val *permutation_resource_identifier(yyjson_mut_doc *doc,
                                                       const MeasureConditionPermutationModel *permutation)
{
    const char *id = measure_condition_permutation_presenter_id(permutation);
    if (doc == NULL || id == NULL) {
        return NULL;
    }
    return jsonapi_resource_identifier(doc, measure_condition_permutation_presenter_resource_type(), id);
}

yyjson_mut_val *measure_condition_permutation_group_jsonapi_resource_identifier(
    yyjson_mut_doc *doc,
    const MeasureConditionPermutationGroupModel *group)
{
    const char *id = measure_condition_permutation_group_presenter_id(group);
    if (doc == NULL || id == NULL) {
        return NULL;
    }
    return jsonapi_resource_identifier(doc, measure_condition_permutation_group_presenter_resource_type(), id);
}

yyjson_mut_val *measure_condition_permutation_group_jsonapi_relationship_data_for_measure(
    yyjson_mut_doc *doc,
    const MeasureConditionPermutationGroupModelList *groups,
    const char *measure_sid)
{
    if (doc == NULL || groups == NULL || measure_sid == NULL) {
        return NULL;
    }

    yyjson_mut_val *data = yyjson_mut_arr(doc);
    if (data == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < groups->len; i++) {
        const MeasureConditionPermutationGroupModel *group = &groups->items[i];
        if (!streq(group->measure_sid, measure_sid)) {
            continue;
        }
        yyjson_mut_val *ref = measure_condition_permutation_group_jsonapi_resource_identifier(doc, group);
        if (ref == NULL || !yyjson_mut_arr_append(data, ref)) {
            return NULL;
        }
    }
    return data;
}

yyjson_mut_val *measure_condition_permutation_jsonapi_resource(
    yyjson_mut_doc *doc,
    const MeasureConditionPermutationModel *permutation)
{
    const char *id = measure_condition_permutation_presenter_id(permutation);
    if (doc == NULL || permutation == NULL || id == NULL) {
        return NULL;
    }

    yyjson_mut_val *resource = jsonapi_resource_object(doc,
                                                       measure_condition_permutation_presenter_resource_type(),
                                                       id);
    yyjson_mut_val *relationships = yyjson_mut_obj(doc);
    yyjson_mut_val *condition_data = yyjson_mut_arr(doc);
    if (resource == NULL || relationships == NULL || condition_data == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < permutation->measure_condition_sid_count; i++) {
        yyjson_mut_val *ref = jsonapi_resource_identifier(doc,
                                                          measure_condition_presenter_resource_type(),
                                                          permutation->measure_condition_sids[i]);
        if (ref == NULL || !yyjson_mut_arr_append(condition_data, ref)) {
            return NULL;
        }
    }

    if (!jsonapi_add_data_relationship(doc, relationships, "measure_conditions", condition_data) ||
        !yyjson_mut_obj_add_val(doc, resource, "relationships", relationships)) {
        return NULL;
    }
    return resource;
}

yyjson_mut_val *measure_condition_permutation_group_jsonapi_resource(
    yyjson_mut_doc *doc,
    const MeasureConditionPermutationGroupModel *group)
{
    const char *id = measure_condition_permutation_group_presenter_id(group);
    if (doc == NULL || group == NULL || id == NULL) {
        return NULL;
    }

    yyjson_mut_val *resource = jsonapi_resource_object(doc,
                                                       measure_condition_permutation_group_presenter_resource_type(),
                                                       id);
    yyjson_mut_val *attributes = yyjson_mut_obj(doc);
    yyjson_mut_val *relationships = yyjson_mut_obj(doc);
    yyjson_mut_val *permutation_data = yyjson_mut_arr(doc);
    if (resource == NULL || attributes == NULL || relationships == NULL || permutation_data == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < group->permutations.len; i++) {
        yyjson_mut_val *ref = permutation_resource_identifier(doc, &group->permutations.items[i]);
        if (ref == NULL || !yyjson_mut_arr_append(permutation_data, ref)) {
            return NULL;
        }
    }
    if (!yyjson_mut_obj_add_strcpy(doc, attributes, "condition_code", group->condition_code) ||
        !jsonapi_add_data_relationship(doc, relationships, "permutations", permutation_data) ||
        !yyjson_mut_obj_add_val(doc, resource, "attributes", attributes) ||
        !yyjson_mut_obj_add_val(doc, resource, "relationships", relationships)) {
        return NULL;
    }
    return resource;
}

int measure_condition_permutation_jsonapi_renderer_self_test(void)
{
    yyjson_mut_doc *doc = yyjson_mut_doc_new(NULL);
    MeasureConditionPermutationModel permutation = {
        .id = "hash",
        .measure_condition_sids = (char *[]){ "789" },
        .measure_condition_sid_count = 1,
    };
    MeasureConditionPermutationGroupModel group = {
        .id = "123-A",
        .measure_sid = "123",
        .condition_code = "A",
        .permutations = {
            .items = &permutation,
            .len = 1,
        },
    };
    int ok = doc != NULL &&
             measure_condition_permutation_jsonapi_resource(doc, &permutation) != NULL &&
             measure_condition_permutation_group_jsonapi_resource(doc, &group) != NULL;
    yyjson_mut_doc_free(doc);
    return ok;
}
