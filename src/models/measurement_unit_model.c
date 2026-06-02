#include "models/measurement_unit_model.h"

#include <stdlib.h>
#include <string.h>

void measurement_unit_model_free(MeasurementUnitModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->measurement_unit_code);
    free(model->description);
    free(model->abbreviation);
    *model = (MeasurementUnitModel){ 0 };
}

void measurement_unit_model_list_free(MeasurementUnitModelList *list)
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->len; i++) {
        measurement_unit_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (MeasurementUnitModelList){ 0 };
}

int measurement_unit_model_list_append(MeasurementUnitModelList *list,
                                       MeasurementUnitModel model)
{
    if (list == NULL) {
        measurement_unit_model_free(&model);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 8 : list->cap * 2;
        MeasurementUnitModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            measurement_unit_model_free(&model);
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

int measurement_unit_model_self_test(void)
{
    MeasurementUnitModelList list = { 0 };
    MeasurementUnitModel model = {
        .measurement_unit_code = copy_literal("KGM"),
        .description = copy_literal("Kilogram"),
        .abbreviation = copy_literal("kg"),
    };

    int ok = model.measurement_unit_code != NULL &&
             model.description != NULL &&
             model.abbreviation != NULL &&
             measurement_unit_model_list_append(&list, model) &&
             list.len == 1 &&
             strcmp(list.items[0].measurement_unit_code, "KGM") == 0;

    measurement_unit_model_list_free(&list);
    return ok;
}
