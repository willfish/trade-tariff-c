#include "models/measure_condition_code_model.h"

#include <stdlib.h>
#include <string.h>

void measure_condition_code_model_free(MeasureConditionCodeModel *model)
{
    if (model == NULL) {
        return;
    }
    free(model->condition_code);
    free(model->description);
    free(model->validity_start_date);
    free(model->validity_end_date);
    *model = (MeasureConditionCodeModel){ 0 };
}

void measure_condition_code_model_list_free(MeasureConditionCodeModelList *list)
{
    if (list == NULL) {
        return;
    }
    for (size_t i = 0; i < list->len; i++) {
        measure_condition_code_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (MeasureConditionCodeModelList){ 0 };
}

int measure_condition_code_model_list_append(MeasureConditionCodeModelList *list,
                                             MeasureConditionCodeModel model)
{
    if (list == NULL) {
        measure_condition_code_model_free(&model);
        return 0;
    }
    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 8 : list->cap * 2;
        MeasureConditionCodeModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            measure_condition_code_model_free(&model);
            return 0;
        }
        list->items = next;
        list->cap = next_cap;
    }
    list->items[list->len++] = model;
    return 1;
}

static char *copy_literal(const char *value)
{
    size_t len = strlen(value);
    char *copy = malloc(len + 1);
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, value, len + 1);
    return copy;
}

int measure_condition_code_model_self_test(void)
{
    MeasureConditionCodeModelList list = { 0 };
    MeasureConditionCodeModel model = {
        .condition_code = copy_literal("B"),
        .description = copy_literal("Presentation of a certificate/licence/document"),
    };
    int ok = model.condition_code != NULL &&
             model.description != NULL &&
             measure_condition_code_model_list_append(&list, model) &&
             list.len == 1 &&
             strcmp(list.items[0].condition_code, "B") == 0;
    measure_condition_code_model_list_free(&list);
    return ok;
}
