#include "models/measure_condition_permutation_model.h"

#include <stdlib.h>
#include <string.h>

static char *copy_literal(const char *value)
{
    if (value == NULL) {
        return NULL;
    }
    size_t len = strlen(value);
    char *copy = malloc(len + 1);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, value, len + 1);
    return copy;
}

void measure_condition_permutation_model_free(MeasureConditionPermutationModel *model)
{
    if (model == NULL) {
        return;
    }
    free(model->id);
    for (size_t i = 0; i < model->measure_condition_sid_count; i++) {
        free(model->measure_condition_sids[i]);
    }
    free(model->measure_condition_sids);
    *model = (MeasureConditionPermutationModel){ 0 };
}

void measure_condition_permutation_model_list_free(MeasureConditionPermutationModelList *list)
{
    if (list == NULL) {
        return;
    }
    for (size_t i = 0; i < list->len; i++) {
        measure_condition_permutation_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (MeasureConditionPermutationModelList){ 0 };
}

int measure_condition_permutation_model_list_append(MeasureConditionPermutationModelList *list,
                                                    MeasureConditionPermutationModel model)
{
    if (list == NULL) {
        measure_condition_permutation_model_free(&model);
        return 0;
    }
    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 4 : list->cap * 2;
        MeasureConditionPermutationModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            measure_condition_permutation_model_free(&model);
            return 0;
        }
        list->items = next;
        list->cap = next_cap;
    }
    list->items[list->len++] = model;
    return 1;
}

int measure_condition_permutation_model_add_condition_sid(MeasureConditionPermutationModel *model,
                                                          const char *measure_condition_sid)
{
    if (model == NULL || measure_condition_sid == NULL) {
        return 0;
    }
    if (model->measure_condition_sid_count == model->measure_condition_sid_cap) {
        size_t next_cap = model->measure_condition_sid_cap == 0 ? 2 : model->measure_condition_sid_cap * 2;
        char **next = realloc(model->measure_condition_sids, next_cap * sizeof(*next));
        if (next == NULL) {
            return 0;
        }
        model->measure_condition_sids = next;
        model->measure_condition_sid_cap = next_cap;
    }
    char *copy = copy_literal(measure_condition_sid);
    if (copy == NULL) {
        return 0;
    }
    model->measure_condition_sids[model->measure_condition_sid_count++] = copy;
    return 1;
}

void measure_condition_permutation_group_model_free(MeasureConditionPermutationGroupModel *model)
{
    if (model == NULL) {
        return;
    }
    free(model->id);
    free(model->measure_sid);
    free(model->condition_code);
    measure_condition_permutation_model_list_free(&model->permutations);
    *model = (MeasureConditionPermutationGroupModel){ 0 };
}

void measure_condition_permutation_group_model_list_free(MeasureConditionPermutationGroupModelList *list)
{
    if (list == NULL) {
        return;
    }
    for (size_t i = 0; i < list->len; i++) {
        measure_condition_permutation_group_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (MeasureConditionPermutationGroupModelList){ 0 };
}

int measure_condition_permutation_group_model_list_append(MeasureConditionPermutationGroupModelList *list,
                                                          MeasureConditionPermutationGroupModel model)
{
    if (list == NULL) {
        measure_condition_permutation_group_model_free(&model);
        return 0;
    }
    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 4 : list->cap * 2;
        MeasureConditionPermutationGroupModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            measure_condition_permutation_group_model_free(&model);
            return 0;
        }
        list->items = next;
        list->cap = next_cap;
    }
    list->items[list->len++] = model;
    return 1;
}

int measure_condition_permutation_model_self_test(void)
{
    MeasureConditionPermutationGroupModelList groups = { 0 };
    MeasureConditionPermutationModel permutation = { .id = copy_literal("abc") };
    MeasureConditionPermutationGroupModel group = {
        .id = copy_literal("123-n/a"),
        .measure_sid = copy_literal("123"),
        .condition_code = copy_literal("n/a"),
    };
    int ok = permutation.id != NULL &&
             group.id != NULL &&
             measure_condition_permutation_model_add_condition_sid(&permutation, "789") &&
             measure_condition_permutation_model_list_append(&group.permutations, permutation) &&
             measure_condition_permutation_group_model_list_append(&groups, group) &&
             groups.len == 1 &&
             groups.items[0].permutations.len == 1 &&
             strcmp(groups.items[0].permutations.items[0].measure_condition_sids[0], "789") == 0;
    measure_condition_permutation_group_model_list_free(&groups);
    return ok;
}
