#include "models/measurement_unit_qualifier_model.h"

#include <stdlib.h>
#include <string.h>

void measurement_unit_qualifier_model_free(MeasurementUnitQualifierModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->measurement_unit_qualifier_code);
    free(model->description);
    *model = (MeasurementUnitQualifierModel){ 0 };
}

void measurement_unit_qualifier_model_list_free(MeasurementUnitQualifierModelList *list)
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->len; i++) {
        measurement_unit_qualifier_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (MeasurementUnitQualifierModelList){ 0 };
}

int measurement_unit_qualifier_model_list_append(MeasurementUnitQualifierModelList *list,
                                                 MeasurementUnitQualifierModel model)
{
    if (list == NULL) {
        measurement_unit_qualifier_model_free(&model);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 8 : list->cap * 2;
        MeasurementUnitQualifierModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            measurement_unit_qualifier_model_free(&model);
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

int measurement_unit_qualifier_model_self_test(void)
{
    MeasurementUnitQualifierModelList list = { 0 };
    MeasurementUnitQualifierModel model = {
        .measurement_unit_qualifier_code = copy_literal("X"),
        .description = copy_literal("per hectolitre"),
    };

    int ok = model.measurement_unit_qualifier_code != NULL &&
             model.description != NULL &&
             measurement_unit_qualifier_model_list_append(&list, model) &&
             list.len == 1 &&
             strcmp(list.items[0].measurement_unit_qualifier_code, "X") == 0;

    measurement_unit_qualifier_model_list_free(&list);
    return ok;
}
