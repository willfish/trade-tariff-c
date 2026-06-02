#include "models/additional_code_model.h"

#include <stdlib.h>
#include <string.h>

void additional_code_model_free(AdditionalCodeModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->additional_code_sid);
    free(model->additional_code_type_id);
    free(model->additional_code);
    free(model->description);
    *model = (AdditionalCodeModel){ 0 };
}

void additional_code_model_list_free(AdditionalCodeModelList *list)
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->len; i++) {
        additional_code_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (AdditionalCodeModelList){ 0 };
}

int additional_code_model_list_append(AdditionalCodeModelList *list,
                                      AdditionalCodeModel model)
{
    if (list == NULL) {
        additional_code_model_free(&model);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 8 : list->cap * 2;
        AdditionalCodeModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            additional_code_model_free(&model);
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

int additional_code_model_self_test(void)
{
    AdditionalCodeModelList list = { 0 };
    AdditionalCodeModel model = {
        .additional_code_sid = copy_literal("123"),
        .additional_code_type_id = copy_literal("8"),
        .additional_code = copy_literal("123"),
        .description = copy_literal("Additional duty"),
    };

    int ok = model.additional_code_sid != NULL &&
             model.additional_code_type_id != NULL &&
             model.additional_code != NULL &&
             model.description != NULL &&
             additional_code_model_list_append(&list, model) &&
             list.len == 1 &&
             strcmp(list.items[0].additional_code_type_id, "8") == 0;

    additional_code_model_list_free(&list);
    return ok;
}
