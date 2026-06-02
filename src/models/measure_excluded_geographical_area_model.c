#include "models/measure_excluded_geographical_area_model.h"

#include <stdlib.h>
#include <string.h>

void measure_excluded_geographical_area_model_free(MeasureExcludedGeographicalAreaModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->measure_sid);
    free(model->geographical_area_sid);
    free(model->geographical_area_id);
    *model = (MeasureExcludedGeographicalAreaModel){ 0 };
}

void measure_excluded_geographical_area_model_list_free(MeasureExcludedGeographicalAreaModelList *list)
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->len; i++) {
        measure_excluded_geographical_area_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (MeasureExcludedGeographicalAreaModelList){ 0 };
}

int measure_excluded_geographical_area_model_list_append(MeasureExcludedGeographicalAreaModelList *list,
                                                         MeasureExcludedGeographicalAreaModel model)
{
    if (list == NULL) {
        measure_excluded_geographical_area_model_free(&model);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 8 : list->cap * 2;
        MeasureExcludedGeographicalAreaModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            measure_excluded_geographical_area_model_free(&model);
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

int measure_excluded_geographical_area_model_self_test(void)
{
    MeasureExcludedGeographicalAreaModelList list = { 0 };
    MeasureExcludedGeographicalAreaModel model = {
        .measure_sid = copy_literal("123"),
        .geographical_area_sid = copy_literal("456"),
        .geographical_area_id = copy_literal("AD"),
    };

    int ok = model.measure_sid != NULL &&
             model.geographical_area_sid != NULL &&
             model.geographical_area_id != NULL &&
             measure_excluded_geographical_area_model_list_append(&list, model) &&
             list.len == 1 &&
             strcmp(list.items[0].geographical_area_id, "AD") == 0;

    measure_excluded_geographical_area_model_list_free(&list);
    return ok;
}
