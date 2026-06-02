#include "models/measure_condition_model.h"

#include <stdlib.h>
#include <string.h>

void measure_condition_model_free(MeasureConditionModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->measure_condition_sid);
    free(model->measure_sid);
    free(model->condition_code);
    free(model->component_sequence_number);
    free(model->condition_duty_amount);
    free(model->condition_monetary_unit_code);
    free(model->condition_measurement_unit_code);
    free(model->condition_measurement_unit_qualifier_code);
    free(model->certificate_type_code);
    free(model->certificate_code);
    free(model->action_code);
    free(model->action);
    free(model->measure_condition_code_description);
    free(model->certificate_type_description);
    free(model->certificate_description);
    free(model->measurement_unit_description);
    free(model->measurement_unit_qualifier_description);
    *model = (MeasureConditionModel){ 0 };
}

void measure_condition_model_list_free(MeasureConditionModelList *list)
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->len; i++) {
        measure_condition_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (MeasureConditionModelList){ 0 };
}

int measure_condition_model_list_append(MeasureConditionModelList *list,
                                        MeasureConditionModel model)
{
    if (list == NULL) {
        measure_condition_model_free(&model);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 8 : list->cap * 2;
        MeasureConditionModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            measure_condition_model_free(&model);
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

int measure_condition_model_self_test(void)
{
    MeasureConditionModelList list = { 0 };
    MeasureConditionModel model = {
        .measure_condition_sid = copy_literal("789"),
        .measure_sid = copy_literal("123"),
        .condition_code = copy_literal("B"),
    };

    int ok = model.measure_condition_sid != NULL &&
             model.measure_sid != NULL &&
             model.condition_code != NULL &&
             measure_condition_model_list_append(&list, model) &&
             list.len == 1 &&
             strcmp(list.items[0].measure_condition_sid, "789") == 0;

    measure_condition_model_list_free(&list);
    return ok;
}
