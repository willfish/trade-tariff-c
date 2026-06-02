#include "models/measure_type_model.h"

#include <stdlib.h>
#include <string.h>

void measure_type_model_free(MeasureTypeModel *model)
{
    if (model == NULL) {
        return;
    }
    free(model->measure_type_id);
    free(model->description);
    free(model->measure_type_series_id);
    free(model->measure_type_series_description);
    free(model->measure_component_applicable_code);
    free(model->order_number_capture_code);
    free(model->trade_movement_code);
    free(model->validity_start_date);
    free(model->validity_end_date);
    *model = (MeasureTypeModel){ 0 };
}

void measure_type_model_list_free(MeasureTypeModelList *list)
{
    if (list == NULL) {
        return;
    }
    for (size_t i = 0; i < list->len; i++) {
        measure_type_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (MeasureTypeModelList){ 0 };
}

int measure_type_model_list_append(MeasureTypeModelList *list, MeasureTypeModel model)
{
    if (list == NULL) {
        measure_type_model_free(&model);
        return 0;
    }
    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 8 : list->cap * 2;
        MeasureTypeModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            measure_type_model_free(&model);
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

int measure_type_model_self_test(void)
{
    MeasureTypeModelList list = { 0 };
    MeasureTypeModel model = {
        .measure_type_id = copy_literal("103"),
        .description = copy_literal("Third country duty"),
        .measure_type_series_id = copy_literal("C"),
    };
    int ok = model.measure_type_id != NULL &&
             model.description != NULL &&
             model.measure_type_series_id != NULL &&
             measure_type_model_list_append(&list, model) &&
             list.len == 1 &&
             strcmp(list.items[0].measure_type_id, "103") == 0;
    measure_type_model_list_free(&list);
    return ok;
}
