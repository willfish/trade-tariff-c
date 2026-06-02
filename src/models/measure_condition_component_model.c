#include "models/measure_condition_component_model.h"

#include <stdlib.h>
#include <string.h>

void measure_condition_component_model_free(MeasureConditionComponentModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->measure_condition_sid);
    free(model->duty_expression_id);
    free(model->duty_amount);
    free(model->monetary_unit_code);
    free(model->measurement_unit_code);
    free(model->measurement_unit_qualifier_code);
    free(model->duty_expression_description);
    *model = (MeasureConditionComponentModel){ 0 };
}

void measure_condition_component_model_list_free(MeasureConditionComponentModelList *list)
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->len; i++) {
        measure_condition_component_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (MeasureConditionComponentModelList){ 0 };
}

int measure_condition_component_model_list_append(MeasureConditionComponentModelList *list,
                                                  MeasureConditionComponentModel model)
{
    if (list == NULL) {
        measure_condition_component_model_free(&model);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 8 : list->cap * 2;
        MeasureConditionComponentModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            measure_condition_component_model_free(&model);
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

int measure_condition_component_model_self_test(void)
{
    MeasureConditionComponentModelList list = { 0 };
    MeasureConditionComponentModel model = {
        .measure_condition_sid = copy_literal("789"),
        .duty_expression_id = copy_literal("01"),
        .duty_amount = copy_literal("8.95"),
    };

    int ok = model.measure_condition_sid != NULL &&
             model.duty_expression_id != NULL &&
             model.duty_amount != NULL &&
             measure_condition_component_model_list_append(&list, model) &&
             list.len == 1 &&
             strcmp(list.items[0].duty_expression_id, "01") == 0;

    measure_condition_component_model_list_free(&list);
    return ok;
}
