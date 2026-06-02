#include "models/measure_model.h"

#include <stdlib.h>
#include <string.h>

void measure_model_free(MeasureModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->measure_sid);
    free(model->goods_nomenclature_sid);
    free(model->goods_nomenclature_item_id);
    free(model->measure_type_id);
    free(model->measure_type_description);
    free(model->geographical_area_sid);
    free(model->geographical_area_id);
    free(model->additional_code_sid);
    free(model->additional_code_type_id);
    free(model->additional_code_id);
    free(model->ordernumber);
    free(model->reduction_indicator);
    free(model->national);
    free(model->trade_movement_code);
    free(model->measure_type_series_id);
    free(model->measure_generating_regulation_role);
    free(model->measure_generating_regulation_id);
    free(model->validity_start_date);
    free(model->validity_end_date);
    free(model->effective_start_date);
    free(model->effective_end_date);
    *model = (MeasureModel){ 0 };
}

void measure_model_list_free(MeasureModelList *list)
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->len; i++) {
        measure_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (MeasureModelList){ 0 };
}

int measure_model_list_append(MeasureModelList *list, MeasureModel model)
{
    if (list == NULL) {
        measure_model_free(&model);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 8 : list->cap * 2;
        MeasureModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            measure_model_free(&model);
            return 0;
        }
        list->items = next;
        list->cap = next_cap;
    }

    list->items[list->len++] = model;
    return 1;
}

int measure_model_import(const MeasureModel *model)
{
    return model != NULL &&
           model->trade_movement_code != NULL &&
           (strcmp(model->trade_movement_code, "0") == 0 ||
            strcmp(model->trade_movement_code, "2") == 0);
}

int measure_model_export(const MeasureModel *model)
{
    return model != NULL &&
           model->trade_movement_code != NULL &&
           (strcmp(model->trade_movement_code, "1") == 0 ||
            strcmp(model->trade_movement_code, "2") == 0);
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

int measure_model_self_test(void)
{
    MeasureModelList list = { 0 };
    MeasureModel model = {
        .measure_sid = copy_literal("123"),
        .goods_nomenclature_sid = copy_literal("456"),
        .measure_type_id = copy_literal("103"),
        .measure_type_description = copy_literal("Third country duty"),
        .geographical_area_id = copy_literal("1011"),
        .trade_movement_code = copy_literal("0"),
    };

    int ok = model.measure_sid != NULL &&
             model.goods_nomenclature_sid != NULL &&
             model.measure_type_id != NULL &&
             model.measure_type_description != NULL &&
             model.geographical_area_id != NULL &&
             model.trade_movement_code != NULL &&
             measure_model_list_append(&list, model) &&
             list.len == 1 &&
             strcmp(list.items[0].measure_type_id, "103") == 0 &&
             measure_model_import(&list.items[0]) &&
             !measure_model_export(&list.items[0]);

    measure_model_list_free(&list);
    return ok;
}
