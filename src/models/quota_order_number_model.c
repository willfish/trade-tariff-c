#include "models/quota_order_number_model.h"

#include <stdlib.h>
#include <string.h>

void quota_order_number_model_free(QuotaOrderNumberModel *model)
{
    if (model == NULL) {
        return;
    }

    free(model->quota_order_number_sid);
    free(model->quota_order_number_id);
    free(model->validity_start_date);
    free(model->validity_end_date);
    *model = (QuotaOrderNumberModel){ 0 };
}

void quota_order_number_model_list_free(QuotaOrderNumberModelList *list)
{
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->len; i++) {
        quota_order_number_model_free(&list->items[i]);
    }
    free(list->items);
    *list = (QuotaOrderNumberModelList){ 0 };
}

int quota_order_number_model_list_append(QuotaOrderNumberModelList *list,
                                         QuotaOrderNumberModel model)
{
    if (list == NULL) {
        quota_order_number_model_free(&model);
        return 0;
    }

    if (list->len == list->cap) {
        size_t next_cap = list->cap == 0 ? 8 : list->cap * 2;
        QuotaOrderNumberModel *next = realloc(list->items, next_cap * sizeof(*next));
        if (next == NULL) {
            quota_order_number_model_free(&model);
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

int quota_order_number_model_self_test(void)
{
    QuotaOrderNumberModelList list = { 0 };
    QuotaOrderNumberModel model = {
        .quota_order_number_sid = copy_literal("123"),
        .quota_order_number_id = copy_literal("054001"),
        .validity_start_date = copy_literal("2024-01-01T00:00:00.000Z"),
    };

    int ok = model.quota_order_number_sid != NULL &&
             model.quota_order_number_id != NULL &&
             model.validity_start_date != NULL &&
             quota_order_number_model_list_append(&list, model) &&
             list.len == 1 &&
             strcmp(list.items[0].quota_order_number_id, "054001") == 0;

    quota_order_number_model_list_free(&list);
    return ok;
}
