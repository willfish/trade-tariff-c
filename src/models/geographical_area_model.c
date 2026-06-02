#include "models/geographical_area_model.h"

#include <stdlib.h>
#include <string.h>

void geographical_area_model_free(GeographicalAreaModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->geographical_area_sid);
    free(model->geographical_area_id);
    free(model->description);
    free(model->parent_geographical_area_sid);
    *model = (GeographicalAreaModel){ 0 };
}

void geographical_area_model_list_free(GeographicalAreaModelList *list)
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->len; i++) {
        geographical_area_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (GeographicalAreaModelList){ 0 };
}

int geographical_area_model_list_append(GeographicalAreaModelList *list,
                                        GeographicalAreaModel model)
{
    if (list == NULL) {
        geographical_area_model_free(&model);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 8 : list->cap * 2;
        GeographicalAreaModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            geographical_area_model_free(&model);
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

int geographical_area_model_self_test(void)
{
    GeographicalAreaModelList list = { 0 };
    GeographicalAreaModel model = {
        .geographical_area_sid = copy_literal("101"),
        .geographical_area_id = copy_literal("1011"),
        .description = copy_literal("All countries"),
    };

    int ok = model.geographical_area_sid != NULL &&
             model.geographical_area_id != NULL &&
             model.description != NULL &&
             geographical_area_model_list_append(&list, model) &&
             list.len == 1 &&
             strcmp(list.items[0].geographical_area_id, "1011") == 0;

    geographical_area_model_list_free(&list);
    return ok;
}
