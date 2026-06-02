#include "models/measure_legal_act_model.h"

#include <stdlib.h>
#include <string.h>

void measure_legal_act_model_free(MeasureLegalActModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->measure_sid);
    free(model->measure_type_id);
    free(model->regulation_id);
    free(model->role);
    free(model->source);
    free(model->validity_start_date);
    free(model->validity_end_date);
    free(model->effective_start_date);
    free(model->effective_end_date);
    free(model->officialjournal_number);
    free(model->officialjournal_page);
    free(model->published_date);
    free(model->information_text);
    *model = (MeasureLegalActModel){ 0 };
}

void measure_legal_act_model_list_free(MeasureLegalActModelList *list)
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->len; i++) {
        measure_legal_act_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (MeasureLegalActModelList){ 0 };
}

int measure_legal_act_model_list_append(MeasureLegalActModelList *list,
                                        MeasureLegalActModel model)
{
    if (list == NULL) {
        measure_legal_act_model_free(&model);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 8 : list->cap * 2;
        MeasureLegalActModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            measure_legal_act_model_free(&model);
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

int measure_legal_act_model_self_test(void)
{
    MeasureLegalActModelList list = { 0 };
    MeasureLegalActModel model = {
        .measure_sid = copy_literal("123"),
        .measure_type_id = copy_literal("103"),
        .regulation_id = copy_literal("R230001"),
        .role = copy_literal("1"),
        .source = copy_literal("generating"),
    };

    int ok = model.measure_sid != NULL &&
             model.measure_type_id != NULL &&
             model.regulation_id != NULL &&
             model.role != NULL &&
             model.source != NULL &&
             measure_legal_act_model_list_append(&list, model) &&
             list.len == 1 &&
             strcmp(list.items[0].regulation_id, "R230001") == 0;

    measure_legal_act_model_list_free(&list);
    return ok;
}
